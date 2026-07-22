# Renderer

## Overview

The renderer is built as a submit-style, forward-rendering pipeline sitting
behind a stable `IRenderer` interface. The interface is a hard module boundary:
backends (e.g. an OpenGL implementation) are compiled into a separate `.dll`
and returned to the core via a factory function
(`CreateRendererFunc`/`DestroyRendererFunc`). Everything crossing that
boundary (handles, descriptors, draw commands) is kept POD/ABI-safe —
no `std::string`, no STL containers with non-trivial layouts.

Layers don't own or call the renderer directly for scene submission. They
push `DrawCommand`s into a per-view queue (`submit`); the actual GPU work
(state changes, draw calls) happens once, in `endFrame`, after commands are
sorted. This lets any number of layers (game scene, debug overlay, UI)
contribute draw calls in any order without stepping on each other.

## Pipeline stages (current)

1. `beginFrame` — renderer-side frame setup
2. Layers call `RenderSystem::submitScene` (and similar) which walk the
   ECS registry, resolve GPU resources, pack materials, and call
   `IRenderer::submit(view, cmd)` — this only enqueues, nothing is drawn yet
3. `endFrame` — for each `View`, sort its queued commands by `sortKey`,
   apply GPU state per `RenderPass` group, execute the actual draw calls
4. `swapBuffers` (core, after `endFrame`)

Not yet implemented, but the architecture already supports them without
interface changes (see "Planned multi-pass" below): shadow pass, post-processing,
lighting.

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

### `RenderPass`

A `RenderPass` (`Shadow`/`Opaque`/`Transparent`/`PostProcess`/`Ui`) is a GPU
state descriptor, not a shader selector — it says "blend on/off, depth write
on/off", nothing about which shader program runs. Different materials with
different shaders can share the same pass (e.g. terrain and characters both
sit in `Opaque`), and the same shader can be used by materials in different
passes (e.g. a PBR shader used by both an opaque wall and a semi-transparent
window material).

`DrawCommand::pass` + `DrawCommand::sortKey` drive this: `sortKey` packs
`pass` into the most significant bits (via bit shifts, e.g. `pass << 56`),
so a single `std::sort` groups same-pass commands together, then
sub-sorts within a pass — by shader/geometry for `Opaque` (minimize state
changes), by camera distance for `Transparent` (correct back-to-front blending).

### Shaders — reflection by name, not fixed structs

A shader is described by a `.shader.json` file (see `formats/shader-format.md`)
alongside its `.vert`/`.frag` sources: vertex attributes (semantic + type,
`location` assigned by array order), uniform parameters (name + type, `offset`
computed automatically per std140 alignment rules), and texture slots
(name + binding index). Any shader can declare any set of parameters —
nothing is hardcoded to a fixed "standard material" struct.

`ShaderDesc::uniformBlockSize`/per-param `offset` follow std140 packing
(GL-specific; a future Vulkan/DX12 backend would need std430/HLSL cbuffer
variants — see `alignStd140()` and its TODO).

model matrices and cameras are transmitted as pre-fixed uniforms: `uModel, uView, uProjection`

### Materials — name-based parameter binding, not fixed fields

```cpp
struct Material {
    TypedAssetID<Shader> shader;
    MaterialBlendMode blendMode; // Opaque / AlphaCutoff / Transparent
    std::unordered_map<std::string, MaterialParamValue> params;
    std::unordered_map<std::string, TypedAssetID<Texture>> textures;
};
```

`packMaterial()` iterates the *shader's* declared uniforms/texture slots and
looks each one up by name in the material — missing entries are zero-filled
(shader-declared defaults are a TODO). This means a shader only receives the
parameters it actually declares; an unlit shader can ignore metallic/roughness
entirely, and a fully custom shader (UI, particles) doesn't need to declare
any of the "standard" model parameters at all.

Standard parameter names produced by the model-import pipeline
(`albedoColor`, `metallic`, `roughness`, `albedoMap`, `metallicMap`,
`normalMap`, `roughnessMap`) are documented in
`formats/standard-material-params.md` — a shader intended to render regular
imported models should declare matching names to receive them.

`MaterialBlendMode` maps to `RenderPass` via `passFromBlendMode()`:
`Opaque`/`AlphaCutoff` → `RenderPass::Opaque` (same GPU state; cutoff is a
shader-side `discard`, not a blend state change — not implemented yet),
`Transparent` → `RenderPass::Transparent`.

Transparency detection during model import prefers an explicit alpha mode
from the source format (glTF's `alphaMode`: OPAQUE/MASK/BLEND) over the
numeric `AI_MATKEY_OPACITY` fallback. Texture pixel scanning is deliberately
not used — too slow/unreliable as a heuristic; trusts explicit content authoring.

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

Reasoning (see `decisions/0002-renderer-owns-present.md`, **not yet written
as a file** — drafted in conversation only): `IWindow` previously called
`swapBuffers()` itself, which works for OpenGL (`glfwSwapBuffers`) but
doesn't generalize — DirectX/Vulkan present through the graphics
API/swapchain (`IDXGISwapChain::Present`, `vkQueuePresent`), not through a
windowing call. Keeping present on the window would force `IWindow` to
carry backend-specific knowledge, defeating the purpose of the `IRenderer`
module boundary. `endFrame` ("finish processing this frame's commands") and
`present` ("show the result to the user") are also kept distinct — useful
if frame recording/execution is ever decoupled (e.g. multi-threaded
rendering with a frame of latency).

Consequence: `IWindow::swapBuffers()` becomes unused/removable; every
`IRenderer` backend must implement `present()`, even a trivial GL one
(`glfwSwapBuffers` under the hood).

**TODO:** save `docs/decisions/0002-renderer-owns-present.md` as a proper
ADR (one decision per file, same format as `0001-graphics-api.md`).

## `GpuResourceRegistry`

Listens for `AssetEvent`s on the `EventBus`. On `Loaded`, builds the
appropriate `gfx::*Desc` from the CPU-side asset and calls the matching
`upload*`, caching the resulting `GpuHandle` under a `GpuResourceKey{AssetType, AssetID}`.
`resolve()` returns a plain (untyped) `GpuHandle`; callers wrap it into the
correct `TypedGpuHandle<Tag>` at the call site.

`Reloaded`/`Unloaded` handling is not yet implemented (TODO).

## Planned multi-pass (architecture supports it, not yet implemented)

Shadow mapping and post-processing don't require interface changes beyond
adding `createRenderTarget`/`destroyRenderTarget` and a
`getRenderTargetTexture(RenderTargetHandle) -> TextureHandle` bridge method.
Both follow the same shape: a separate `View` (different camera and/or
render target) whose output texture is read by a later `View`'s shader as
an ordinary texture slot, submitted in explicit, hand-written order
(no render graph / automatic dependency scheduling — deliberately deferred
until more than 2-3 sequential passes make manual ordering unwieldy).

```
View "shadow"  (light camera  → shadow-map texture)
View "scene"   (player camera → offscreen color texture)
View "postfx"  (fullscreen quad → backbuffer, samples "scene" texture)
```

Lighting (how light sources reach a shader) is deliberately not designed yet.

## Known limitations / TODO

- `IRenderer` is still one flat class — the discussed split into
  `IRendererLifecycle`/`IGpuResourceUploader`/`IViewManager`/`IFrameRenderer`
  hasn't been applied yet
- `docs/decisions/0002-renderer-owns-present.md` hasn't been saved as an
  actual file yet — only drafted in conversation
- Upload methods still return a bare `TypedGpuHandle`, not the discussed
  `UploadResult<HandleT>{handle, success}` + `getLastError()` pair
- `GpuResourceRegistry`: `Reloaded`/`Unloaded` asset events not handled
- No shadow mapping (architecture supports it; not implemented)
- No post-processing pass (architecture supports it; not implemented)
- No lighting model / light source passing to shaders yet
- `AlphaCutoff` blend mode is declared but currently behaves identically to `Opaque`
  (no shader-side discard convention established yet)
- std140 uniform packing is GL-specific; a second backend (Vulkan/DX12) needs
  its own alignment rule (see `alignStd140()`)
- No render graph / automatic pass dependency ordering — multi-pass order is
  hand-written; fine for a couple of passes, will need revisiting if the
  pipeline grows to several sequential passes
- No `OpenGLRenderer` (or any) concrete `IRenderer` implementation yet —
  everything above is interface + game-side systems only