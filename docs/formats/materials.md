# Materials

Covers the `Material` struct/asset format and `MaterialInstance` (per-object
overrides). For the shader-side contract a material's shader must satisfy
(`targetPass`, geometry vs forward), see
[`architecture/shaders.md`](../architecture/shaders.md). For the renderer
pipeline that consumes materials, see
[`architecture/renderer.md`](../architecture/renderer.md).

## `Material` — name-based parameter binding, not fixed fields

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
[`standard-material-params.md`](standard-material-params.md) — a shader
intended to render regular imported models should declare matching names to
receive them.

`MaterialBlendMode` maps to `RenderPass` via `passFromBlendMode()`: today,
`Opaque`/`AlphaCutoff` → `RenderPass::Opaque` (same GPU state; cutoff is a
shader-side `discard`, not a blend state change — not implemented yet),
`Transparent` → `RenderPass::Transparent`. Under the planned deferred+forward
pipeline (see `architecture/shaders.md`), `blendMode` additionally selects
which shader *contract* (`"geometry"` vs `"forward"`) a material's shader
must satisfy, validated against the shader's `targetPass`.

Transparency detection during model import prefers an explicit alpha mode
from the source format (glTF's `alphaMode`: OPAQUE/MASK/BLEND) over the
numeric `AI_MATKEY_OPACITY`/base-color-alpha fallback. Texture pixel scanning
is deliberately not used — too slow/unreliable as a heuristic; trusts
explicit content authoring.

---

## `Material` as a JSON asset on disk

**Status: decided, not yet implemented** — today `Material` is only built
programmatically (e.g. `ModelLoader`/`GameLayer::registerDefaultAssets`);
there is no loader for a standalone `.material.json` file yet.

```json
{
  "shader": "assets/shaders/materials/brick_wall.shader.json",
  "blendMode": "Opaque",
  "params": {
    "albedoColor": [1.0, 0.3, 0.2, 1.0],
    "roughness": 0.6,
    "metallic": 0.0
  },
  "textures": {
    "albedoMap": "assets/textures/brick_albedo.png"
  }
}
```

Requirements for the loader:
- `shader` is resolved through `AssetManager`/`getOrLoad<Shader>`.
- If `params` contains a name absent from the target shader's
  `shaderDesc.uniforms`, log a warning at load time (otherwise it's a silent
  desync — the parameter is just dropped in `packMaterial`, with no
  indication why).
- Users can add any custom parameter name without engine changes —
  `packMaterial` is already fully generic (looks up by name from
  `shaderDesc.uniforms` in `material.params`).

---

## `MaterialInstance` — per-object override on top of a shared `Material`

**Status: decided, not yet implemented.** See
`../decisions/0006-material-instance.md`.

Problem: `Material` is a shared asset — several objects in a scene may
reference the same `Material`, so a parameter can't be changed directly on
the asset without affecting every object using it.

Solution: a new lightweight class, **not** an asset, stored as part of an
object's scene component (e.g. `MeshRendererComponent`), holding only a diff
on top of the base material:

```cpp
struct MaterialInstance {
    TypedAssetID<Material> base;
    std::unordered_map<std::string, MaterialParamValue> overrides; // only overridden fields
    // (similarly could have texture overrides, if/when needed)

    void setFloat(const std::string &name, float value);
    void setVec4(const std::string &name, const Vec4 &value);
    void clearOverride(const std::string &name);
};
```

New packing function — an extension of `packMaterial`, lookup priority:
first `instance.overrides`, if not found — `baseMaterial.params`, if still
not found — default (zero), as in the current implementation. Same rule
applies to textures.

### Open question — explicitly deferred (not designed/implemented now)

**Material parameter animation** (`MaterialAnimatorSystem`, time-based lerp)
was raised only as a hypothetical "what if this is needed later" question.
Answer: yes, implementable on top of `MaterialInstance` via a separate
game-side system (tentatively `MaterialAnimatorSystem`), not built into
`Material`/`MaterialInstance` itself. Noted here as a future possibility,
not a planned/scheduled feature.
