# Shader Descriptor Format (`.shader.json`)

Every shader consists of three files sharing the same base name:

```
pbr.vert          # vertex shader source
pbr.frag          # fragment shader source
pbr.shader.json   # metadata: attributes, uniforms, texture slots
```

`ShaderLoader` only takes the base path (`assets/shaders/pbr`) and appends the three extensions itself.

---

## File structure

```json
{
  "attributes": [
    { "semantic": "POSITION", "type": "Vec3" },
    { "semantic": "NORMAL", "type": "Vec3" },
    { "semantic": "TEXCOORD0", "type": "Vec2" }
  ],
  "uniforms": [
    { "name": "albedoColor", "type": "Vec4" },
    { "name": "roughness", "type": "Float" },
    { "name": "metallic", "type": "Float" }
  ],
  "textureSlots": [
    { "name": "albedoMap", "slot": 0 },
    { "name": "normalMap", "slot": 1 }
  ]
}
```

All three sections are optional — if a shader has no textures, just omit `"textureSlots"`.

---

## `attributes` — vertex layout

Describes what the shader expects as input (`layout(location = N) in ...` in GLSL).

| Field | Type | Description |
|---|---|---|
| `semantic` | string | Attribute name: `"POSITION"`, `"NORMAL"`, `"TEXCOORD0"`, `"TANGENT"`, etc. — a free-form string, the engine doesn't validate it against a fixed list |
| `type` | string | `Float`, `Vec2`, `Vec3`, `Vec4` |

⚠️ **`location` is NOT specified explicitly** — it's assigned automatically based on array order (first element = location 0, second = location 1, ...). This means **the order in the JSON must match the order of `layout(location = N)` in the actual `.vert` file**. Reordering entries in the JSON won't throw an error at load time — the data will simply end up bound to the wrong attributes.

```glsl
// pbr.vert — locations must follow the same order as the JSON above
layout(location = 0) in vec3 aPosition;  // attributes[0]
layout(location = 1) in vec3 aNormal;    // attributes[1]
layout(location = 2) in vec2 aUV;        // attributes[2]
```

---

## `uniforms` — material parameters

Describes the variables that `Material` will fill in by name. There's no need to specify `offset` — it's computed automatically following **std140** rules (GL uniform buffer layout).

| Field | Type | Description |
|---|---|---|
| `name` | string | Parameter name; `Material` refers to it using this same string |
| `type` | string | `Float`, `Int`, `Vec2`, `Vec3`, `Vec4`, `Mat4` |

⚠️ **std140 padding**: `Vec3` is aligned like `Vec4` (16 bytes), which can create "holes" in memory between fields. If minimizing blob size matters, group `Vec4`/`Mat4` fields together and keep small `Float`/`Int` fields separate.

Matching uniform block in the shader:

```glsl
layout(std140, binding = 0) uniform MaterialBlock {
    vec4 albedoColor;   // uniforms[0]
    float roughness;    // uniforms[1]
    float metallic;     // uniforms[2]
};
```

If a material doesn't set a value for a parameter, it defaults to zero (TODO: shader-declared default values aren't supported yet).

> 🔧 This rule (std140) is GL-specific. If a Vulkan/DX12 backend is added in the future, it will need its own padding rule (std430 / HLSL cbuffer) — see `alignStd140()` in code.

---

## `textureSlots` — texture bindings

| Field | Type | Description |
|---|---|---|
| `name` | string | Slot name; `Material` refers to it using this string |
| `slot` | int | Binding index, matches `DrawCommand::textures[slot]`. Max is `kMaxTextureSlots` (currently 8) |

```glsl
layout(binding = 0) uniform sampler2D uAlbedoMap; // textureSlots[0], slot=0
layout(binding = 1) uniform sampler2D uNormalMap; // textureSlots[1], slot=1
```

---

## Example material using this shader

```cpp
Material mat;
mat.shader = pbrShaderId;
mat.params["albedoColor"] = MaterialParamValue{{1.0f, 0.2f, 0.2f, 1.0f}};
mat.params["roughness"] = MaterialParamValue{{0.6f}};
mat.textures["albedoMap"] = brickTextureId;
```

Names in `Material::params`/`Material::textures` must match the strings from `.shader.json` — if a name isn't found, the uniform is zero-filled or the texture slot is left empty, with no error raised.