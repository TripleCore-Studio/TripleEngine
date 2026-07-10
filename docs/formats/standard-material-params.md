# Standard Model Material Contract

When a model is imported via `ModelLoader`, the resulting `Material` always
populates the following parameter names. A shader intended to render regular
3D models should declare matching entries in its `.shader.json` to receive them.

## Uniforms

| Name | Type | Description |
|---|---|---|
| `albedoColor` | Vec4 | Base color tint; `.a` is opacity (1.0 = fully opaque) |
| `metallic` | Float | Metallic factor, 0-1 |
| `roughness` | Float | Roughness factor, 0-1 |

## Textures

| Name | Description |
|---|---|
| `albedoMap` | Diffuse/base color texture |
| `metallicMap` | Metallic texture |
| `normalMap` | Tangent-space normal map |
| `roughnessMap` | Roughness texture |

A shader is free to ignore any of these (e.g. an unlit shader only needs
`albedoMap`), or to be a completely custom, non-model shader (UI, particles,
debug lines) that doesn't declare any of them at all — the model pipeline
simply won't populate parameters that don't exist in the shader's declared uniforms.