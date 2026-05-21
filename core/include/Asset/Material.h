#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vec4.h"
#include "Shader.h"
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset {
    struct Material : public Asset {
    public:
        Material() {}

        TripleMath::Vec4 albedoColor = { 1, 1, 1, 1 };
        float metallic = 0.0f;
        float roughness = 1.0f;

        AssetID albedoTextureId = UINT32_MAX;
        AssetID normalTextureId = UINT32_MAX;
        AssetID metallicTextureId = UINT32_MAX;
        AssetID roughnessTextureId = UINT32_MAX;

        AssetID shaderId = UINT32_MAX;

        Material(const Material& other) = default;
        Material& operator=(const Material& other) = default;
    };
}

#endif // MATERIAL_H
