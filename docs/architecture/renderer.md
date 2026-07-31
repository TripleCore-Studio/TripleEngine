# Renderer

## Overview

The renderer sits behind a stable `IRenderer` interface. The interface is a
hard module boundary: backends (e.g. an OpenGL implementation) are compiled
into a separate `.dll` and returned to the core via a factory function
(`CreateRendererFunc`/`DestroyRendererFunc`). Everything crossing that
boundary (handles, descriptors, draw commands) is kept POD/ABI-safe —
no `std::string`, no STL containers with non-trivial layouts.

Layers don't own or call the renderer directly for scene submission. They
push `DrawCommand`s into a per-view queue (`submit`); the actual GPU work
(state changes, draw calls) happens once, in `endFrame`, after commands are
sorted. This lets any number of layers (game scene, debug overlay, UI)
contribute draw calls in any order without stepping on each other. Multiple
`View`s can be queued per frame via `setViewOrder` — this is how multi-pass
rendering (shadow maps, deferred G-buffer + lighting, post-processing) is
built, entirely out of the same primitives, without new `IRenderer` methods.

This document covers the rendering pipeline's core primitives (`View`,
`RenderPass`, `RenderTarget`, `IRenderer`). Shader authoring/composition
(contracts, `#pragma include`, lighting) lives in
[`shaders.md`](shaders.md); the `Material`/`MaterialInstance` asset format
lives in [`formats/materials.md`](../formats/materials.md).

> **Rendering model — currently implemented vs. planned:** what's actually
> built today is a single-pass forward pipeline (one `View`, `Opaque` and
> `Transparent` `RenderPass` groups sharing it). The architecture has since
> been extended on paper to a **Deferred (Opaque/Cutoff) + Forward
> (Transparent) hybrid** — see "Planned pipeline" below and
> `decisions/0004-deferred-opaque-forward-transparent.md`. That design is not
> yet implemented in code; this document describes both so implementers have
> the full target shape to build toward.

## Pipeline stages (current, single forward pass)

1. `beginFrame` — renderer-side frame setup
2. Layers call `RenderSystem::submitScene` (and similar) which walk the
   ECS registry, resolve GPU resources, pack materials, and call
   `IRenderer::submit(view, cmd)` — this only enqueues, nothing is drawn yet
3. `endFrame` — for each `View` (in `setViewOrder`), sort its queued commands
   by `sortKey`, apply GPU state per `RenderPass` group, execute the actual
   draw calls
4. `present()` (core, after `endFrame`)

This is the shape actually running today: one `View` targeting the
backbuffer, `Opaque` and `Transparent` commands sorted and drawn in it. See
"Planned pipeline" below for the deferred+forward replacement.

## Key concepts

### Handles

`GpuHandle{slot, generation}` — a generation-checked handle, not a raw index/pointer.
Prevents stale references resolving to a reused slot after a resource is unloaded.

Wrapped per-resource-type via `TypedGpuHandle<Tag>` (`TextureHandle`, `ShaderHandle`,
`GeometryHandle`, `ViewHandle`, `RenderTargetHandle`) so the compiler rejects
mixing up e.g. a texture handle passed where a shader handle is expected.

### `View`

A `View` is "where we're looking from and where the result goes" — a camera
plus a `RenderTargetHandle` (backbuffer or an offscreen texture) plus a
viewport/clear state. A new `View` is needed whenever the camera or the
render target changes (e.g. a shadow-casting light's viewpoint, or an
offscreen scene pass feeding a post-process pass). Multiple `RenderPass`
groups (opaque, transparent) commonly coexist within a single `View`.

Multiple `View`s may also target the **same** `RenderTargetHandle` — this is
how the planned deferred G-buffer, lighting, and transparent-forward passes
share one physical FBO while writing to different attachments (see "Planned
pipeline" → `ViewDesc` below).

### `RenderPass`

A `RenderPass` is a GPU state descriptor, not a shader selector — it says
"blend on/off, depth write on/off", nothing about which shader program runs.
Different materials with different shaders can share the same pass (e.g.
terrain and characters both sit in `Opaque`), and the same shader can be
used by materials in different passes (e.g. a PBR shader used by both an
opaque wall and a semi-transparent window material).

Current values: `Shadow`, `Opaque`, `Transparent`, `PostProcess`, `UI`,
`Count`. The planned pipeline adds `Lighting` (see below) — a fullscreen
pass reading the G-buffer, GPU state similar to `PostProcess` (depth
test/write off, blend off) but kept semantically distinct from it.

`DrawCommand::pass` + `DrawCommand::sortKey` drive this: `sortKey` packs
`pass` into the most significant bits (via bit shifts, e.g. `pass << 56`),
so a single `std::sort` groups same-pass commands together, then
sub-sorts within a pass — by shader/geometry for `Opaque` (minimize state
changes), by camera distance for `Transparent` (correct back-to-front blending).

What runs inside each pass (shader contracts, `Opaque`/`AlphaCutoff` vs
`Transparent` split) is covered in [`shaders.md`](shaders.md).

### Geometry — raw bytes, not a fixed `Vertex` struct

`GeometryDesc` takes a raw byte buffer + stride, not a typed vertex array.
The `Model` asset stores `vertexData` (raw bytes) + a `VertexLayout`
(list of `VertexAttributeDesc`: semantic, type, offset, computed without
std140 rules — just tightly packed). This exists specifically so a shader
isn't limited to one fixed vertex format; a custom shader can declare
whatever attributes it needs, as long as some loader produces geometry
containing them.

`ModelLoader`/`AssimpHelper` currently always produce a fixed attribute set
(POSITION/NORMAL/TANGENT/BITANGENT/TEXCOORD0) — this is a property of the
Assimp import path, not a limitation the engine imposes on shaders in general.

> **TODO(vertex-layout binding):** today the fixed 5-attribute layout is matched to
> a shader purely by both sides hand-coding the same numeric `location` (see
> `makeStandardVertexLayout()` in `ModelLoader.cpp` and the `layout(location=N)`
> qualifiers in the `.vert`) — nothing actually cross-checks them by `semantic`.
> Drop or reorder an attribute on either side (as happened with
> TANGENT/BITANGENT) and it silently mismatches instead of failing to compile.
> Two follow-ups, not yet done:
> 1. `ModelLoader` should emit only the attributes it actually computed for a
>    given mesh (e.g. skip TANGENT/BITANGENT when no UVs/no tangent generation
>    ran) instead of always emitting the full standard set.
> 2. Attribute binding should be resolved by matching `semantic` between the
>    geometry's `VertexLayout` and the shader's declared attributes (reflection,
>    the same way `textureSlots`/uniforms already work) rather than by both
>    sides independently hardcoding the same `location` number.
>
> This is explicitly deferred (see "Open questions" below) — the planned
> pipeline still uses the single fixed `makeStandardVertexLayout()`.

A `Model` is one shared vertex/index buffer; `Mesh`/`Primitive` reference
`indexOffset`/`indexCount` ranges within it, each with its own material —
one `uploadGeometry()` call per model regardless of how many materials/parts
it has.

### Frame-scoped memory — `FrameArena`

A bump allocator (`allocate()` advances an offset, `reset()` rewinds it to
zero) used to hold packed material uniform blobs for the current frame only.
Owned by core (in `EngineContext`, alongside the renderer/window/bus), reset
once per frame before the render phase — not owned by individual layers,
since only core knows the true frame boundary. Data written here is only
valid until the next `reset()`; the renderer must consume it synchronously
within `submit`/`endFrame`, not hold onto the pointer across frames.

---

## Planned pipeline: Deferred (Opaque/Cutoff) + Forward (Transparent)

**Status: decided, not yet implemented.** See
`decisions/0004-deferred-opaque-forward-transparent.md` for the ADR. Shader
authoring/composition details for this pipeline (material contracts,
`#pragma include`, light data, the lighting pass file) are in
[`shaders.md`](shaders.md) — this section covers only the renderer-side
primitives: `RenderTarget`/MRT, `ViewDesc`, and the per-frame `View` order.

### Why hybrid, not pure forward or pure deferred

Deferred shading computes lighting once per screen pixel instead of once per
object/material per light, and its cost doesn't scale linearly with light
count per draw call the way naive forward shading's does. That's the whole
reason to adopt it for opaque geometry.

Deferred fundamentally doesn't work for transparency, though: a G-buffer
holds exactly one layer of surface data per pixel, and there's no slot for
"what's visible behind this transparent surface" — compositing multiple
overlapping transparent layers requires seeing what's underneath, which a
single G-buffer sample can't provide. So `Transparent` materials keep
computing lighting themselves and are rendered in a separate **forward**
pass, drawn after the deferred lighting result is ready, blending on top of
it.

### `RenderTarget` — MRT (multiple color attachments per FBO)

Today `GLRenderTargetRes` holds a single `colorTexture`. The G-buffer needs
several color attachments on the **same** physical FBO — one `RenderTargetHandle`
stays one FBO with multiple "canvases" inside it, not several separate render
targets.

```cpp
struct RenderTargetDesc {
    RenderTargetType type = RenderTargetType::BackBuffer;
    uint32_t width = 0, height = 0;
    std::vector<TextureFormat> colorFormats; // was: one colorFormat, now: a list (MRT)
    bool hasDepth = true;
};

struct GLRenderTargetRes {
    GLuint fbo = 0;
    std::vector<gfx::TextureHandle> colorTextures; // was: one colorTexture
    gfx::TextureHandle depthTexture;
    int width = 0, height = 0;
    bool isBackBuffer = false;
};
```

`IRenderer::getRenderTargetTexture` gains an attachment index:

```cpp
[[nodiscard]] virtual TextureHandle
getRenderTargetTexture(RenderTargetHandle handle, uint32_t attachmentIndex = 0) const = 0;
```

G-buffer layout for the main case:

```
attachment 0: gAlbedo   (Rgba8)    — albedo * albedoColor
attachment 1: gNormal   (Rgba16F)  — world-space normal, packed *0.5+0.5
attachment 2: gMaterial (Rgba8)    — r=roughness, g=metallic, b=ao, a=emissiveStrength
attachment 3: gLitColor (Rgba16F)  — Lighting pass output, later composited with by Transparent pass
+ one shared depth buffer for the whole FBO
```

### `ViewDesc` — which attachments are active, and clear behavior

Several `View`s may reference the **same** `RenderTargetHandle` but with a
different set of active color attachments for writing (`glDrawBuffers`) and
different clear behavior:

```cpp
struct ViewDesc {
    CameraData camera;
    RenderTargetHandle target;
    Viewport viewport;
    Color clearColor;
    bool clearDepth = true;
    std::vector<uint32_t> activeColorAttachments; // NEW: e.g. {0,1,2} for gbuffer, {3} for lighting/transparent
    bool clearColorAttachments = true;             // NEW: lighting/transparent must NOT clear (false)
};
```

### Full per-frame `View` order (`setViewOrder`)

```
setViewOrder({
    shadowView,         // target: a separate depth-only RenderTarget (already implemented)

    gbufferView,        // target: m_gbufferTarget, activeColorAttachments={0,1,2}
                        // draws Opaque/AlphaCutoff objects → write gAlbedo/gNormal/gMaterial

    lightingView,       // target: m_gbufferTarget, activeColorAttachments={3}, clearColorAttachments=false
                        // fullscreen quad, ONE DrawCommand for the whole View
                        // reads gAlbedo/gNormal/gMaterial/gDepth (as input textures) + shadow map + LightsBuffer (SSBO)
                        // depth test/write OFF, writes lit color into gLitColor

    transparentView,    // target: m_gbufferTarget, activeColorAttachments={3}, clearColorAttachments=false
                        // draws Transparent objects forward, on top of the already-resolved gLitColor
                        // depth test ON (against the same depth buffer from gbufferView), depth write OFF, blend ON

    postProcessView,    // target: getBackBufferTarget()
                        // fullscreen quad, reads gLitColor (attachment 3 of m_gbufferTarget) as an input texture
                        // applies tone mapping/gamma correction/effects, writes straight to the backbuffer
                        // RenderPass::PostProcess (already existing, no applyPassState changes)
});
```

`present()` afterward is just `glfwSwapBuffers` — no separate blit step,
since the backbuffer write already happened in `postProcessView`.

Important: **`gbufferView`, `lightingView`, and `transparentView` all point
at the same `RenderTargetHandle`** — one physical FBO; the only difference
between them is which attachments are active for writing in that `View`
(via `glDrawBuffers`) and what's read as an input texture. There's one
shared depth buffer across all three; it isn't recreated or cleared between
them (except in `gbufferView`, where it's cleared at the start).

Who actually assembles this `View` order and uploads the system shaders that
run in each pass is covered in [`shaders.md`](shaders.md) — short version:
`RenderSystem` (game/engine layer), not `OpenGLRenderer`.

---

## `IRenderer`

Current actual interface (single class, not yet physically split — see TODO below):

```cpp
class IRenderer {
public:
	virtual ~IRenderer() = default;

	// lifecycle
	virtual bool initialize(const RendererConfig &config) = 0;
	virtual void shutdown() = 0;
	virtual void resize(uint32_t width, uint32_t height) = 0;

	// resources
	[[nodiscard]] virtual TextureHandle uploadTexture(const TextureDesc &desc) = 0;
	[[nodiscard]] virtual ShaderHandle uploadShader(const ShaderDesc &desc) = 0;
	[[nodiscard]] virtual GeometryHandle uploadGeometry(const GeometryDesc &desc) = 0;

	virtual void unloadTexture(TextureHandle handle) = 0;
	virtual void unloadShader(ShaderHandle handle) = 0;
	virtual void unloadGeometry(GeometryHandle handle) = 0;

	// render targets
	[[nodiscard]] virtual RenderTargetHandle
	createRenderTarget(const RenderTargetDesc &desc) = 0;
	virtual void destroyRenderTarget(RenderTargetHandle handle) = 0;
	[[nodiscard]] virtual TextureHandle
	getRenderTargetTexture(RenderTargetHandle handle) const = 0;

	// views
	[[nodiscard]] virtual ViewHandle createView(const ViewDesc &desc) = 0;
	virtual void destroyView(ViewHandle handle) = 0;
	virtual void updateView(ViewHandle handle, const ViewDesc &desc) = 0;
	[[nodiscard]] virtual RenderTargetHandle getBackBufferTarget() const = 0;

	// frame
	virtual void beginFrame(float time) = 0;
	virtual void submit(ViewHandle view, const DrawCommand &cmd) = 0;
	virtual void endFrame() = 0;
	virtual void present() = 0;
};

using CreateRendererFunc = IRenderer *(*)();
using DestroyRendererFunc = void (*)(IRenderer *);
```

> Under the planned pipeline, `RenderTargetDesc` gains `colorFormats`
> (replacing the single `colorFormat`) and `getRenderTargetTexture` gains an
> `attachmentIndex` parameter — see "Planned pipeline" → `RenderTarget` above.
> Not reflected in the interface above yet.

Methods still conceptually group into four consumer roles, matching who
actually calls them — this grouping is currently just comments/section
ordering within the one class, not separate C++ interfaces:

- **lifecycle** — `initialize`/`shutdown`/`resize` (called by core/Application)
- **resources** — `upload*`/`unload*` (called by `GpuResourceRegistry`)
- **views** — `createView`/`updateView`/`destroyView`/`getBackBufferTarget` (called by layers)
- **frame** — `beginFrame`/`submit`/`endFrame`/`present` (called by `RenderSystem`/core)

**TODO (discussed, not yet applied):** physically split into
`IRendererLifecycle`/`IGpuResourceUploader`/`IViewManager`/`IFrameRenderer`,
with `IRenderer` publicly inheriting all four, so consumers that only need
one role (e.g. `GpuResourceRegistry`) can take a narrower pointer type
instead of the full interface. Also discussed: upload methods returning a
POD `UploadResult<HandleT>{handle, success}` instead of a bare handle (no
`std::string` in it — ABI-unsafe across the DLL boundary), with error text
via a separate `getLastError() -> const char*`, read and copied by the
caller immediately after a failed upload. Neither change is reflected in
the code above yet.

### Presentation (`present()`)

`present()` is owned by the renderer, not the window — core calls it right
after `endFrame()`:

```cpp
m_impl->renderer->beginFrame(m_lastTime);
for (auto &layer : m_layerStack)
	layer->onRender(m_lastTime);
m_impl->renderer->endFrame();
m_impl->renderer->present(); // instead of m_impl->window->swapBuffers()
```

See `decisions/0002-renderer-owns-present.md` for the full ADR.
Consequence: `IWindow::swapBuffers()` becomes unused/removable; every
`IRenderer` backend must implement `present()`, even a trivial GL one
(`glfwSwapBuffers` under the hood).

## `GpuResourceRegistry`

Listens for `AssetEvent`s on the `EventBus`. On `Loaded`, builds the
appropriate `gfx::*Desc` from the CPU-side asset and calls the matching
`upload*`, caching the resulting `GpuHandle` under a `GpuResourceKey{AssetType, AssetID}`.
`resolve()` returns a plain (untyped) `GpuHandle`; callers wrap it into the
correct `TypedGpuHandle<Tag>` at the call site.

`Reloaded`/`Unloaded` handling is not yet implemented (TODO).

## Shadow mapping (implemented as a separate `View` + offscreen `RenderTarget`)

A separate `View` (different camera, offscreen depth `RenderTarget`) whose
output texture is read by a later `View`'s shader as an ordinary texture
slot, submitted in explicit, hand-written order (no render graph / automatic
dependency scheduling — deliberately deferred until more than a handful of
sequential passes make manual ordering unwieldy). Only a small fixed number
of lights may cast shadows in a single frame — see
`decisions/0003-shadow-casting-light-limit.md`.

Post-processing and the full deferred lighting pipeline follow the same
"separate `View`, output read as a texture slot" shape — see "Planned
pipeline" above for the concrete `View` order once lighting/deferred is
implemented.

## Open questions — explicitly deferred (not designed/implemented now)

- **UI pass** — the command format for UI hasn't been designed yet (likely a
  separate submission path, not through `Material`/`DrawCommand` as they
  stand today).
- **Per-model vertex layout** (a model dictates its own attribute set) —
  deferred in favor of the current fixed `makeStandardVertexLayout()` (see
  the vertex-layout binding TODO above).
- **Multi-shading-model deferred** (different lighting models for different
  materials within one G-buffer, via an extra `materialID` channel) —
  considered only as a hypothetical edge of complexity, not a plan.

Shader/material-specific open questions (runtime std140 validation, material
parameter animation) are listed in [`shaders.md`](shaders.md) and
[`formats/materials.md`](../formats/materials.md) respectively.

## Known limitations / TODO

- `IRenderer` is still one flat class — the discussed split into
  `IRendererLifecycle`/`IGpuResourceUploader`/`IViewManager`/`IFrameRenderer`
  hasn't been applied yet
- Upload methods still return a bare `TypedGpuHandle`, not the discussed
  `UploadResult<HandleT>{handle, success}` + `getLastError()` pair
- `GpuResourceRegistry`: `Reloaded`/`Unloaded` asset events not handled
- The deferred (Opaque/Cutoff) + forward (Transparent) pipeline described
  above is designed but **not implemented** — today's renderer still runs a
  single forward `View`
- No post-processing pass yet (architecture supports it; not implemented)
- No lighting model / light source passing to shaders implemented yet
  (design in [`shaders.md`](shaders.md); SSBO-based, three light types)
- `AlphaCutoff` blend mode is declared but currently behaves identically to `Opaque`
  (no shader-side discard convention established yet)
- std140 uniform packing is GL-specific; a second backend (Vulkan/DX12) needs
  its own alignment rule (see `alignStd140()`)
- No render graph / automatic pass dependency ordering — multi-pass order is
  hand-written; fine for a handful of passes, will need revisiting if the
  pipeline grows further
