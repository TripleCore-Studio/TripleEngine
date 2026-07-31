# Shader Descriptor Format (`.shader.json`)

This file only applies to **material shaders** (see
[`architecture/shaders.md`](../architecture/shaders.md)) — system shaders
(shadow, lighting pass, postprocess) are loaded via
`ShaderLoader::loadSystemShader` and don't have a `.shader.json` at all,
since they never go through `Material`/`packMaterial`.

Every material shader consists of three files sharing the same base name:

```
pbr.vert          # vertex shader source
pbr.frag          # fragment shader source
pbr.shader.json   # metadata: targetPass, uniforms, texture slots
```

`ShaderLoader` only takes the base path (`assets/shaders/pbr`) and appends the three extensions itself.

---

## File structure

```json
{
  "targetPass": "geometry",
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

All sections are optional — if a shader has no textures, just omit
`"textureSlots"`. Vertex attribute layout is **not** described here — it's
fixed in code (`makeStandardVertexLayout()`), not per-shader; see
`architecture/renderer.md` → Geometry. (An earlier version of this format
had an `"attributes"` field here — it never actually drove vertex layout and
has been removed.)

---

## `targetPass` — shader contract (planned, not yet implemented)

**Status: decided, not yet implemented** — see
`architecture/shaders.md` → "Planned: material shader contracts" and
`decisions/0004-deferred-opaque-forward-transparent.md`.

Declares which of the two material shader contracts this shader implements,
so the loader can validate it against the owning `Material.blendMode`:

| Value | Used by `blendMode` | Contract |
|---|---|---|
| `"geometry"` | `Opaque`, `AlphaCutoff` | Writes G-buffer outputs (`gAlbedo`/`gNormal`/`gMaterial`), no lighting, no `o_FragColor` |
| `"forward"` | `Transparent` | Computes lighting itself, writes `o_FragColor` with alpha, ordinary alpha blending |

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