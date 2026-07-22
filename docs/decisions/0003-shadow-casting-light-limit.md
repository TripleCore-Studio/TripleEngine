# 0003: Shadow-Casting Light Limit

## Status
Accepted

## Context
The renderer supports shadows via a separate `View` + offscreen `RenderTarget`
per shadow-casting light (see "Planned multi-pass" in renderer.md). Each such
`View` requires a full additional geometry pass over shadow-casting scene
objects. Scenes can contain many light sources (streetlights, torches,
handheld lights, etc.), but rendering a full shadow pass for every one of
them is not feasible — cost scales linearly with shadow-casting light count,
and each pass re-submits and re-sorts a significant part of the scene.

## Decision
Only a small, fixed maximum number of lights (4-5) may be marked as
shadow-casting in a single frame, regardless of how many light sources exist
in the scene. All other lights contribute to lighting (once lighting is
designed — see TODO in renderer.md) without casting shadows.

This mirrors standard practice in real-time engines: a directional light
(sun/moon) is almost always shadow-casting, and a handful of nearby/important
point or spot lights (e.g. a handheld flashlight in a dark scene) may be
added on top, but "every light casts a shadow" is not attempted.

## Consequences
- Only explicitly marked lights get a dedicated shadow `View` + `RenderTarget`
  + entry in `setViewOrder`; this must happen before the light contributes
  shadows, and is a manual/game-side decision, not automatic.
- Non-shadow-casting lights need a separate, cheaper lighting path once
  lighting is designed (position/color/radius only, no additional render pass).
- Games needing many simultaneous shadow-casting lights (e.g. many dynamic
  point-light shadows) are not served well by this design and would need a
  different technique (e.g. shadow atlases, virtual shadow maps) — deferred
  until a concrete need arises.