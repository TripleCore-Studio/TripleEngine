# Deferred rendering: status

Branch: `feature/deferred-rendering` (from `develop`).

## Build state

- `0e78f6d` Refactor OpenGL renderer with pooled resource management: last commit in `develop` that builds, pure forward rendering.
- `600db38` Add MaterialInstance, MRT support and shader docs: does NOT build (first deferred work, committed unfinished).
- `491b5f6` .. `90997a8` (this branch): `90997a8` is the first commit where the whole project builds again.

## Done

- `ViewDesc::activeColorAttachments` / `clearColorAttachments` (MRT selection per view).
- `RenderPass::Lighting` added.
- GL renderer: per-view draw buffers, back buffer target created internally, resource code split into `OpenGLResource.cpp`.
- Editor creates meshes through `EntityFactory` (fixes empty `materialInstances`).

## Current behaviour

Rendering still works as forward: main view uses a single attachment `{0}`.

## Remaining

- TODO: fill in (G-buffer render target, geometry pass shaders, lighting pass, switching the main view to deferred, ...).

## Notes

- `core/src/base/Engine.cpp` has a local `vsync = false` debug tweak that is not committed.
