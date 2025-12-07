#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include "Vec3.h"

namespace TripleEngineCore::Graphics {
    struct Material {
        std::string name;

        TripleMath::Vec3 albedoColor = { 1, 1, 1 };
        float metallic = 0.0f;
        float roughness = 1.0f;

        std::string albedoTexturePath;
        std::string normalTexturePath;
        std::string metallicTexturePath;
        std::string roughnessTexturePath;
    };
}

#endif // MATERIAL_H
