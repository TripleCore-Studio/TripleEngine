# Shaders

This document covers shaders as a concept: how they're described, the
contract split between deferred-geometry and forward-transparent shaders,
GLSL composition via `#pragma include`, light data, and who loads/assembles
what. For the renderer's core primitives (`View`, `RenderPass`,
`RenderTarget`), see [`renderer.md`](renderer.md). For the `Material`/
`MaterialInstance` asset format, see
[`formats/materials.md`](../formats/materials.md).

## Shaders — reflection by name, not fixed structs

A shader is described by a `.shader.json` file (see
[`formats/shader-format.md`](../formats/shader-format.md)) alongside its
`.vert`/`.frag` sources: uniform parameters (name + type, `offset` computed
automatically per std140 alignment rules) and texture slots (name + binding
index). Any shader can declare any set of parameters — nothing is hardcoded
to a fixed "standard material" struct.

Vertex attribute layout is **not** described in `.shader.json` (the format's
old `attributes` field never actually drove anything and has been dropped);
it's fixed in code via `makeStandardVertexLayout()`, matched to a shader's
`layout(location = N)` qualifiers by both sides hardcoding the same numbers
(see the vertex-layout binding TODO in `renderer.md` → Geometry).

`ShaderDesc::uniformBlockSize`/per-param `offset` follow std140 packing
(GL-specific; a future Vulkan/DX12 backend would need std430/HLSL cbuffer
variants — see `alignStd140()` and its TODO).

Model matrices and cameras are transmitted as pre-fixed uniforms: `uModel`,
`uView`, `uProjection`.

---

## Planned: material shader contracts (Deferred + Forward hybrid)

**Status: decided, not yet implemented** — part of
`decisions/0004-deferred-opaque-forward-transparent.md`. See `renderer.md` →
"Planned pipeline" for the `View`/`RenderTarget` side of this.

A material's shader contract now depends on `Material.blendMode`, via a new
`targetPass` field in `.shader.json`:

```json
{
  "targetPass": "geometry",
  "uniforms": [ ... ],
  "textureSlots": [ ... ]
}
```

Values: `"geometry"` (for `Opaque`/`AlphaCutoff`) or `"forward"` (for
`Transparent`). This lets the loader validate that `Material.blendMode`
isn't out of sync with its shader (a `Transparent` material shouldn't
accidentally reference a geometry-contract shader, and vice versa).

### `"geometry"` contract (Opaque / AlphaCutoff)

Writes into the G-buffer, does **not** compute lighting at all, does **not**
write `o_FragColor`:

```glsl
layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gMaterial;

void main() {
    gAlbedo = vec4(texture(albedoMap, vUV).rgb * albedoColor.rgb, 1.0);
    gNormal = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
    gMaterial = vec4(roughness, metallic, 1.0, 0.0);
}
```

`AlphaCutoff` additionally `discard`s by alpha, same set of outputs.

### `"forward"` contract (Transparent)

Computes lighting itself, reads `LightsBuffer`/`FrameBlock` directly (via
`#pragma include`, see below), writes a normal `o_FragColor` with alpha,
followed by ordinary alpha blending:

```glsl
void main() {
    vec3 albedo = texture(albedoMap, vUV).rgb * albedoColor.rgb;
    vec3 N = normalize(vNormal);
    vec3 V = normalize(u_camPos - vFragPos);
    vec3 lit = vec3(0.0);
    for (int i = 0; i < u_lightCount; i++) {
        // loop over lights[i], same principle as the lighting pass
        lit += ComputePBR(albedo, roughness, metallic, N, V, L, radiance);
    }
    o_FragColor = vec4(lit, albedoAlpha);
}
```

Why deferred doesn't work for transparency at all (and why `Transparent`
keeps its own forward lighting instead): a G-buffer holds exactly one layer
of surface data per pixel, and there's no slot for "what's visible behind
this transparent surface" — compositing overlapping transparent layers
requires seeing what's underneath, which a single G-buffer sample can't
provide.

---

## `#pragma include` — a GLSL composition mechanism, no hidden codegen

**Status: decided, not yet implemented.** See
`decisions/0005-shader-pragma-include.md`.

Rejected alternative: the engine silently wraps the user's `main()` with
shared UBO declarations — bad, because the author can't see what names are
available to them, and it'd require a custom editor with autocomplete to be
usable at all.

Decision: textual preprocessing on the CPU before shader compilation,
analogous to C/C++'s `#include` (GLSL itself doesn't have a fully-supported
native `#include` — `GL_ARB_shading_language_include` is barely implemented
anywhere in practice).

- `#pragma include "path/relative/to/root.glsl"` — handled by a new class
  (tentatively `ShaderPreprocessor`), recursively, with an include-guard (the
  set of already-inserted paths, to avoid duplication/cycles).
- The include base directory is a new `RendererConfig` field (tentatively
  `shaderRootDir`).
- Applies identically to material shaders and system shaders (shadow,
  lighting pass, postprocess) — preprocessing doesn't care where a
  `ShaderDesc` came from.
- If a user includes a chunk and also declares an entity with the same name,
  that's an ordinary GLSL redefinition compile error — no special handling
  needed; the existing `getLastError()` already surfaces it.

### Shared "contract" chunks

- `common/frame_ubo.glsl` — `FrameBlock` (`binding = 1`): `u_view`,
  `u_projection`, `u_camPos`, `u_time`, `u_lightDir`, `u_lightIntensity`,
  `u_lightColor` (updated once per frame, unlike `MaterialBlock` at
  `binding = 0`, which is updated per draw call via the already-implemented
  orphaning trick).
- `common/gbuffer_samplers.glsl` — fixed G-buffer sampler names: `gAlbedo`,
  `gNormal`, `gMaterial`, `gDepth`. Used by the lighting pass and, if needed,
  postprocess.
- `common/lights_ssbo.glsl` — the fixed light-source struct (see below) and
  SSBO layout.
- `common/pbr_lighting.glsl` — an optional `ComputePBR(...)` function; a
  shader author MAY skip including it and write their own lighting function
  under any other name/signature (a toon-shading example was explicitly
  discussed as a valid alternative path) — the library is optional, not
  mandatory.

### File layout on disk

```
assets/shaders/
├── common/                          # include chunks — a contract, not standalone Shader assets
│   ├── frame_ubo.glsl
│   ├── gbuffer_samplers.glsl
│   ├── lights_ssbo.glsl
│   └── pbr_lighting.glsl            # optional
│
├── materials/                       # user material shaders: .vert + .frag + .shader.json
│   ├── brick_wall.vert / .frag / .shader.json          (targetPass: geometry)
│   ├── foliage_cutoff.vert / .frag / .shader.json      (targetPass: geometry, AlphaCutoff)
│   └── glass.vert / .frag / .shader.json               (targetPass: forward, Transparent)
│
└── system/                          # engine system shaders: only .vert + .frag, NO .shader.json
    ├── shadow_opaque.vert / .frag
    ├── shadow_cutoff.vert / .frag
    ├── lighting_pass.vert / .frag   # user-editable file, but no JSON (see below)
    └── postprocess.vert / .frag
```

---

## Light sources — SSBO (not UBO), three standard types

**Status: decided, not yet implemented** (part of
`decisions/0004-deferred-opaque-forward-transparent.md`).

SSBO over UBO is a deliberate choice: the number of lights in a scene is
dynamic (not a small fixed set like material parameters), which UBOs handle
poorly (either wasted memory for a `MAX_LIGHTS` cap, or a hard compile-time
limit). SSBOs give `std430` layout and a buffer that can grow without a
shader-compile-time cap.

Supported types: **Directional, Point, Spot** (the standard set; area
lights and other exotic types were deliberately not considered this session
— not part of the data structure for now).

Upload uses the same orphaning trick already implemented for
`m_materialUbo`, just with `GL_SHADER_STORAGE_BUFFER` and a different
binding index, updated once per frame (not per draw call, unlike
`MaterialBlock`).

The same SSBO is read by **both** the lighting pass (for Opaque/Cutoff,
resolved via the G-buffer) and the Transparent materials' forward shaders
(which compute lighting directly in their own `main()`) — both walk the same
light array, no data duplication.

## Lighting pass — an editable file asset, not C++ code, no JSON

**Status: decided, not yet implemented.**

The lighting formula (the lighting pass's `main()` body) is an ordinary text
file (`assets/shaders/system/lighting_pass.frag`), whose path is set in
`RendererConfig` (e.g. `lightingPassShaderPath`) and can be overridden by the
user with their own file if they want a different lighting model (a custom
BRDF, baking SSAO directly into the lighting pass, etc.).

The fixed, non-negotiable contract is **only** the input names/layout
(`common/gbuffer_samplers.glsl`, `common/lights_ssbo.glsl`) — not the formula
itself.

A `.shader.json` descriptor for the lighting pass is **not** needed and not
created: the lighting pass doesn't go through `Material`/`packMaterial`, it
has no per-instance arbitrary parameters that would need a schema — all its
inputs are already fixed by the G-buffer/SSBO structure. Same applies to
shadow shaders and the postprocess shader.

## Shader loading — two paths, `ShaderLoader` needs extending

**Status: decided, not yet implemented.**

The existing `ShaderLoader::load(params)`, requiring all three files
(`.vert`, `.frag`, `.shader.json`), remains correct **only** for material
shaders. A second entry point is needed for system shaders, which have no
JSON:

```
ShaderLoader::loadSystemShader(basePath) — loads only .vert + .frag,
    does NOT require .shader.json; uniforms/textureSlots stay empty
    (default), since a system shader never goes through packMaterial.
```

Used for: `shadow_opaque`, `shadow_cutoff`, `lighting_pass`, `postprocess`
(and, in the future, `ui_quad`, once the UI pass is designed).

## Who assembles system shaders and the G-buffer — `RenderSystem`, not `OpenGLRenderer`

**Status: decided, not yet implemented.** Important clarification,
correcting an earlier (incorrect) assumption floated in discussion:
`OpenGLRenderer` stays a thin executor of the public `IRenderer` API and
must not "know about" deferred/G-buffer/lighting-pass as concepts. It just
executes the `RenderTargetHandle`/`ViewHandle`/`ShaderHandle`/`DrawCommand`s
it's given through the public methods, in the order set by `setViewOrder`.

Assembling the whole pipeline — loading system shaders via
`ShaderLoader::loadSystemShader`, calling `uploadShader`, creating the
G-buffer via `createRenderTarget` with multiple `colorFormats`, creating all
the `View`s, calling `setViewOrder` — happens in the game/engine layer
(`RenderSystem`), consistent with the already-established pattern of
creating shadow `View`s in `GameLayer::onAttach`, not `onRender`.

`OpenGLRenderer` only holds internally what's strictly necessary for its own
internal mechanics independent of any particular game/scene (e.g.
`m_materialUbo` — the shared material-parameter UBO, the backbuffer target
created in `initialize()`) — it does not hold references to shadow/lighting/
postprocess shaders or the G-buffer target; that's the calling layer's
responsibility.

## Open questions — explicitly deferred (not designed/implemented now)

- **Runtime std140 validation** — cross-checking a shader's actual uniform
  block layout against `ShaderDesc.uniformBlockSize` via
  `glGetActiveUniformBlockiv` at `uploadShader` time — an idea, not
  scheduled.
