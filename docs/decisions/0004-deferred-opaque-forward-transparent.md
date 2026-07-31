# 0004: Deferred (Opaque/Cutoff) + Forward (Transparent) Hybrid Rendering

## Status
Accepted

## Context
The renderer currently runs a single forward pass: every object's shader
computes lighting itself, once per draw call. This scales poorly as light
count grows — cost is roughly `O(objects × lights)` per frame, since each
material/shader combination redoes the lighting loop independently.

Deferred shading computes lighting once per screen pixel (via a G-buffer +
a single fullscreen lighting pass) instead of once per object, decoupling
lighting cost from object count. But deferred fundamentally can't represent
transparency: a G-buffer holds exactly one layer of surface data per pixel,
with no way to store "what's visible behind this transparent surface" —
compositing overlapping transparent layers requires seeing what's
underneath, which a single G-buffer sample can't provide.

## Decision
Adopt a hybrid pipeline:
- **Opaque** and **AlphaCutoff** materials render into a multi-attachment
  G-buffer (deferred geometry pass), followed by a single fullscreen
  **Lighting** pass that resolves lit color once per pixel.
- **Transparent** materials render in a separate **forward** pass, after the
  Lighting pass, computing lighting themselves and blending on top of the
  already-resolved lit image.

This requires: a new `RenderPass::Lighting` value, MRT support on
`RenderTarget` (multiple color attachments per FBO), `ViewDesc` gaining
per-`View` active-attachment and clear control (so multiple `View`s can
target the same physical FBO), an SSBO-based light list (shared by both the
Lighting pass and Transparent forward shaders), and a `targetPass`
(`"geometry"`/`"forward"`) field on material shaders so the loader can catch
a `Material.blendMode` / shader-contract mismatch.

Full details: `architecture/renderer.md` → "Planned pipeline", and
`architecture/shaders.md` → "Planned: material shader contracts".

## Reasons
- Decouples lighting cost from object/material count for the common case
  (opaque scene geometry), which is where light count actually matters most
- Keeps transparency correct and simple (ordinary forward alpha blending),
  rather than attempting a deferred-transparency workaround (e.g. multiple
  G-buffer layers, OIT) that adds significant complexity for comparatively
  few transparent objects in a typical scene
- `OpenGLRenderer` doesn't need new concepts to support this — it's built
  entirely out of existing primitives (`View`, `RenderTarget`, `RenderPass`,
  `DrawCommand`) plus MRT, which stays a renderer-primitive extension, not a
  deferred-specific one

## Consequences
- `RenderTargetDesc`/`GLRenderTargetRes` change shape: `colorFormat` →
  `colorFormats` (a list), and `getRenderTargetTexture` needs an attachment
  index parameter
- `ViewDesc` gains `activeColorAttachments` and `clearColorAttachments` —
  several `View`s can now share one `RenderTargetHandle`
- Material shaders split into two mutually exclusive contracts (geometry vs
  forward) instead of one uniform shape — existing material shaders (all
  currently forward-style, writing `o_FragColor` directly) need rewriting to
  the geometry contract if they're `Opaque`/`AlphaCutoff`
- `RenderSystem` (game/engine layer), not `OpenGLRenderer`, is responsible
  for assembling the G-buffer, system shaders, and `setViewOrder` — keeps
  `OpenGLRenderer` a thin, deferred-agnostic executor of the public
  `IRenderer` API
- Not yet implemented — this ADR records the decision and target shape, not
  a completed change
