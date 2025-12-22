#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include "Vec3.h"
#include "Shader.h"

namespace TripleEngineCore::Asset {
    struct Material {
    public:
        Material() {}

        TripleMath::Vec3 albedoColor = { 1, 1, 1 };
        float metallic = 0.0f;
        float roughness = 1.0f;

        uint32_t albedoTextureIndex = UINT32_MAX;
        uint32_t normalTextureIndex = UINT32_MAX;
        uint32_t metallicTextureIndex = UINT32_MAX;
        uint32_t roughnessTextureIndex = UINT32_MAX;

        uint32_t shaderIndex = UINT32_MAX;

        Material(const Material& other) = default;
        Material& operator=(const Material& other) = default;
    };
}

#endif // MATERIAL_H
