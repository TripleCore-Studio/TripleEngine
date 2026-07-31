# Triple Engine Docs

## Architecture
- [Renderer](architecture/renderer.md)
- [Shaders](architecture/shaders.md)

## File Formats
- [Shader descriptor (.shader.json)](formats/shader-format.md)
- [Standard material params](formats/standard-material-params.md)
- [Materials (Material / MaterialInstance)](formats/materials.md)

## Decisions (ADRs)
- [0001: Graphics API choice](decisions/0001-graphics-api.md)
- [0002: Renderer owns present/swap](decisions/0002-renderer-owns-present.md)
- [0003: Shadow-casting light limit](decisions/0003-shadow-casting-light-limit.md)
- [0004: Deferred (Opaque/Cutoff) + Forward (Transparent) hybrid](decisions/0004-deferred-opaque-forward-transparent.md)
- [0005: GLSL composition via `#pragma include`](decisions/0005-shader-pragma-include.md)
- [0006: `MaterialInstance` for per-object overrides](decisions/0006-material-instance.md)