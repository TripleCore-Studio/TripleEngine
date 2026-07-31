# 0005: GLSL Composition via `#pragma include`

## Status
Accepted

## Context
The deferred+forward pipeline (see `0004-deferred-opaque-forward-transparent.md`)
introduces shared GLSL contracts that many shaders need simultaneously: a
per-frame uniform block (camera/time/sun), fixed G-buffer sampler names, and
a shared light-source SSBO layout. Every material shader (and every system
shader — shadow, lighting pass, postprocess) needs access to some subset of
these without hand-copying declarations between files.

GLSL itself has no reliably-supported native include mechanism —
`GL_ARB_shading_language_include` exists on paper but is barely implemented
in practice across drivers, so it isn't a viable option.

One alternative considered and rejected: have the engine silently wrap a
user's `main()` with injected UBO/SSBO declarations at shader-load time.
Rejected because the shader author can no longer see, just by reading their
own file, which names/types are available to them — this effectively
requires a custom shader editor with autocomplete/documentation to be usable
at all, which is a much larger commitment than the problem calls for.

## Decision
CPU-side textual preprocessing before shader compilation, analogous to C/C++
`#include`:

```glsl
#pragma include "common/frame_ubo.glsl"
#pragma include "common/lights_ssbo.glsl"
```

Handled by a new `ShaderPreprocessor` component, applied uniformly to every
`ShaderDesc` (material and system shaders alike) before it reaches
`uploadShader`. Recursive, with an include-guard (tracking already-inserted
paths) to prevent duplicate insertion/cycles. The include search root is a
new `RendererConfig` field.

Shared chunks (`common/frame_ubo.glsl`, `common/gbuffer_samplers.glsl`,
`common/lights_ssbo.glsl`) define fixed contracts. An additional
`common/pbr_lighting.glsl` chunk providing a `ComputePBR(...)` helper is
explicitly optional — a shader author may write their own lighting function
under a different name/signature (e.g. for toon shading) without including
it.

Name collisions between an included chunk and a shader's own declarations
are left as ordinary GLSL compile errors (redefinition) — no special
detection needed, since `getLastError()` already surfaces GLSL compiler
output.

## Reasons
- Shader authors see exactly what's declared, in plain GLSL, by opening the
  included file — no hidden engine-side codegen step to reason about
- Mirrors a pattern C/C++ developers already know (`#include`), minimizing
  new concepts
- Applies uniformly to material and system shaders, so shared contracts
  (G-buffer sampler names, light SSBO layout) aren't duplicated by hand
  between them
- Avoids depending on `GL_ARB_shading_language_include`, which isn't
  reliably available

## Consequences
- Adds a CPU-side preprocessing step to `uploadShader` (or just before it) —
  every shader's source now passes through `ShaderPreprocessor` first
- `RendererConfig` grows a new field for the include search root
- Shared contract files (`common/*.glsl`) become a de facto stable API —
  changing a name/layout inside one is a breaking change for every shader
  that includes it
- Not yet implemented — this ADR records the decision and target shape, not
  a completed change
