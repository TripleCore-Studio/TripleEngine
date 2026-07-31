# 0006: `MaterialInstance` for Per-Object Material Overrides

## Status
Accepted

## Context
`Material` is (or, with `0004`/JSON-asset loading, will be) a shared asset —
multiple objects in a scene commonly reference the same `Material` (e.g.
many instances of the same brick wall piece). Changing a parameter directly
on a shared `Material` would affect every object referencing it, which is
rarely what's wanted when only one specific object needs, say, a tinted
color or a different roughness value.

## Decision
Introduce `MaterialInstance` — a lightweight, non-asset struct stored as
part of an object's own scene component (e.g. `MeshRendererComponent`),
holding a `base` reference to the shared `Material` plus a sparse map of
`overrides` (only the parameters actually overridden for this object):

```cpp
struct MaterialInstance {
    TypedAssetID<Material> base;
    std::unordered_map<std::string, MaterialParamValue> overrides;

    void setFloat(const std::string &name, float value);
    void setVec4(const std::string &name, const Vec4 &value);
    void clearOverride(const std::string &name);
};
```

Packing (`packMaterialInstance`, an extension of `packMaterial`) resolves
each shader-declared parameter with this priority: `instance.overrides` →
`baseMaterial.params` → zero-default (matching today's `packMaterial`
fallback behavior). The same override-then-base lookup applies to textures
if/when texture overrides are added.

Full format details: `formats/materials.md` → `MaterialInstance`.

## Reasons
- Keeps `Material` genuinely shareable (many objects, one GPU-resource
  identity, one asset to edit) without sacrificing per-object customization
- Sparse overrides mean most objects using a shared material pay no extra
  memory/lookup cost beyond the (usually empty) `overrides` map
- Fits directly into the existing name-based `packMaterial` lookup pattern —
  no new binding mechanism, just an extra lookup tier ahead of it

## Consequences
- `MeshRendererComponent` (or equivalent) needs to carry a `MaterialInstance`
  instead of (or alongside) a bare `Material` reference
- A new `packMaterialInstance` packing path is needed, layered on top of the
  existing `packMaterial`
- Explicitly out of scope for this decision: time-based animation of
  material parameters (a `MaterialAnimatorSystem` lerping `MaterialInstance`
  overrides over time) — noted as a plausible future addition built *on top*
  of `MaterialInstance`, not designed or scheduled here
- Not yet implemented — this ADR records the decision and target shape, not
  a completed change
