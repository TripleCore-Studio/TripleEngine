# Renderer

Short overview. For exact types and signatures, read the headers in
`engine/gfx/include/triple/gfx/` (start with `IRenderer.h`). This file explains
the ideas, not the API.

## Status

| Part | State |
|---|---|
| `IRenderer` + OpenGL backend (DLL) | done |
| Views, render targets, MRT (`colorFormats`, `activeColorAttachments`) | done |
| `RenderPass::Lighting` state | done (state only) |
| Game pipeline | one forward `View` to the backbuffer (`GameLayer::configureRenderPipeline`) |
| G-buffer, lighting pass, forward transparent pass | not built yet, see [ADR 0004](../decisions/0004-deferred-opaque-forward-transparent.md) |
| Shadow `View` | not built yet (only the `Shadow` pass state exists) |
| Post-process, UI pass | not built yet |

## Module boundary

The renderer is the `IRenderer` interface. Backends (now: OpenGL) live in a
separate DLL and are created through `createRenderer` / `destroyRenderer`.
Everything that crosses this border must be POD / ABI-safe: no `std::string`
in results, no STL types with unstable layout. Errors are read with
`getLastError()`.

## How a frame works

1. `beginFrame`
2. Layers walk the scene (`RenderSystem::submitScene`) and call
   `submit(view, DrawCommand)`. This only puts commands in a queue. Nothing is drawn.
3. `endFrame` draws all views in the order given by `setViewOrder`. Each view's
   commands are sorted by `sortKey` first.
4. `present()` (the renderer owns it, see [ADR 0002](../decisions/0002-renderer-owns-present.md))

Because drawing happens once in `endFrame`, any number of layers (game, debug,
UI) can submit in any order. Multi-pass rendering (shadows, deferred, post) is
just several `View`s. It needs no new `IRenderer` methods.

## Main ideas

- **Handles.** `GpuHandle{slot, generation}`, wrapped per type
  (`TextureHandle`, `ShaderHandle`, ...). The generation stops a stale handle
  from pointing at a reused slot. The wrappers stop mixing types.
- **View.** A camera + a render target + viewport + clear settings. Several
  views may use the same render target. They only differ in which color
  attachments they write (`activeColorAttachments`) and which they clear
  (`clearColorAttachments`).
- **RenderPass.** Only GPU state (depth test, depth write, blend). It does not
  choose a shader. `Opaque` and `Transparent` differ by state, not by shader.
  The pass is stored in the top bits of `sortKey`, so one sort groups by pass.
  Inside a pass: `Opaque` sorts by shader/geometry, `Transparent` by distance
  (back to front).
- **Render target.** One `RenderTargetHandle` = one FBO with 0..N color
  attachments and optional depth. `getRenderTargetTexture(handle, index)` and
  `getRenderTargetDepthTexture(handle)` give the textures to read in later views.
- **Geometry.** Raw bytes + stride + `VertexLayout`, not a fixed vertex struct.
  A `Model` is one shared vertex/index buffer. Each mesh part is an
  `indexOffset`/`indexCount` range.
- **FrameArena.** A bump allocator for packed material uniforms. Owned by core,
  reset once per frame. Data is valid only until the next reset, so the
  renderer must use it inside `submit`/`endFrame`.
- **Resources.** The OpenGL backend keeps them in pools (`engine/gl/.../resource/`).
  `GpuResourceRegistry` listens to asset events and uploads assets.
  `Loaded` is handled. `Reloaded` and `Unloaded` are not.

## Target frame (deferred + forward)

Not built yet. This is the plan from ADR 0004.

```
shadowView       -> depth-only target
gbufferView      -> gbuffer target, attachments {0,1,2}   Opaque / AlphaCutoff
lightingView     -> gbuffer target, attachments {3}, no clear   fullscreen quad
transparentView  -> gbuffer target, attachments {3}, no clear   forward, blend on, depth write off
postProcessView  -> backbuffer   tone mapping, gamma
```

G-buffer attachments: `0` albedo (Rgba8), `1` world normal packed `*0.5+0.5`
(Rgba16F), `2` roughness/metallic/ao/emissive (Rgba8), `3` lit color
(Rgba16F). One shared depth buffer.

`RenderSystem` (game layer) builds this pipeline, not `OpenGLRenderer`. The
backend only executes the views it is given.

## Known limitations

- `IRenderer` is one flat class. A split by role (lifecycle, resources, views,
  frame) was discussed, not done.
- Vertex attributes match shaders only by the same `location` number on both
  sides. Nothing checks `semantic`. If one side changes, it fails silently.
- `AlphaCutoff` behaves like `Opaque`. No `discard` convention yet.
- std140 packing is OpenGL-specific (`alignStd140`). Another backend needs its own rule.
- No render graph. The view order is written by hand.
- The UI command format is not designed.
