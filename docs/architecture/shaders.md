# Shaders

Short overview of how shaders work in the engine. For the renderer side
(`View`, `RenderPass`, render targets) see [renderer.md](renderer.md). For file
formats see [shader-format.md](../formats/shader-format.md) and
[materials.md](../formats/materials.md).

## Status

| Part | State |
|---|---|
| `.shader.json` with uniforms and texture slots, std140 offsets | done |
| Fixed uniforms `uModel`, `uView`, `uProjection` | done |
| Material uniform block (`binding = 0`), updated per draw | done |
| `targetPass` in `.shader.json` (`geometry` / `forward`) | not built yet |
| `#pragma include` ([ADR 0005](../decisions/0005-shader-pragma-include.md)) | not built yet |
| Lights SSBO, lighting pass, system shaders (`loadSystemShader`) | not built yet |

The only shader now is `assets/shaders/default.*`.

## How a shader is described

A shader is `.vert` + `.frag` + `.shader.json`. The JSON lists uniform
parameters (name, type) and texture slots (name, binding). Offsets are computed
by std140 rules. Nothing is hardcoded to a "standard material", so any shader
can have any parameters. The engine finds them by name.

The vertex layout is **not** in the JSON. It is fixed in code
(`makeStandardVertexLayout()`) and matched to the shader by the same
`layout(location = N)` numbers. See "Known limitations" in renderer.md.

Camera and model matrices are plain uniforms with fixed names: `uModel`,
`uView`, `uProjection`. The renderer sets them if the shader has them.

## Planned: two shader contracts

Decided in [ADR 0004](../decisions/0004-deferred-opaque-forward-transparent.md).
`targetPass` in `.shader.json` must match the material's `blendMode`. The loader
should reject a mismatch.

- **`geometry`** (`Opaque`, `AlphaCutoff`): writes the G-buffer and does no
  lighting. Outputs: `gAlbedo`, `gNormal`, `gMaterial`. `AlphaCutoff` also uses
  `discard` on alpha.
- **`forward`** (`Transparent`): computes lighting itself, reads the lights, and
  writes `o_FragColor` with alpha.

Why transparent stays forward: a G-buffer keeps one surface layer per pixel.
Transparency needs to see what is behind it, so deferred cannot do it.

## Planned: `#pragma include`

Text preprocessing on the CPU before compile, like C `#include` (GLSL has no
usable built-in one). Works for material shaders and system shaders alike. The
author sees exactly which names come from which file. No hidden code is added
around `main()`.

Shared chunks in `assets/shaders/common/`:

- `frame_ubo.glsl`: per-frame data (`binding = 1`)
- `gbuffer_samplers.glsl`: `gAlbedo`, `gNormal`, `gMaterial`, `gDepth`
- `lights_ssbo.glsl`: light struct and buffer
- `pbr_lighting.glsl`: optional `ComputePBR(...)`. A shader may write its own lighting instead.

## Planned: lights

Lights live in an SSBO, not a UBO, because the count is dynamic. Types:
directional, point, spot. Uploaded once per frame. The lighting pass and the
forward transparent shaders read the same buffer.

## Planned: system shaders

Engine shaders (`shadow_*`, `lighting_pass`, `postprocess`) live in
`assets/shaders/system/`. They have only `.vert` + `.frag`, with no JSON,
because they never go through `Material`. They are loaded with a new
`ShaderLoader::loadSystemShader(basePath)`. The lighting pass file may be
replaced by the user to change the lighting model. Only its inputs are fixed.

`RenderSystem` loads and uploads them and builds the views. `OpenGLRenderer`
knows nothing about deferred or lighting.

## Open questions

- Check the real std140 block layout at `uploadShader` time
  (`glGetActiveUniformBlockiv`). Not scheduled.
