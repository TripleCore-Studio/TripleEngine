#ifndef RUNTIME_MATERIAL_H
#define RUNTIME_MATERIAL_H

#include <Vec3.h>
#include "RuntimeTexture.h"
#include "RuntimeShader.h"

namespace TripleEngineCore::Runtime {
    struct RuntimeMaterial
    {
    public:
        RuntimeMaterial() : metallic(0.0f), roughness(1.0f) {}
		RuntimeMaterial(TripleMath::Vec3 ab, float metallic, float roughness,
            RuntimeTexture abtex, RuntimeTexture normtex, RuntimeTexture metallictex,
            RuntimeTexture roughnesstex, RuntimeShader shader)
            : albedoColor(ab),
            metallic(metallic),
            roughness(roughness),
            albedoTexture(abtex),
			normalTexture(normtex),
			metallicTexture(metallictex),
            roughnessTexture(roughnesstex),
            shader(shader) {}

        TripleMath::Vec3 albedoColor;
        float metallic;
        float roughness;

        RuntimeTexture albedoTexture;
        RuntimeTexture normalTexture;
        RuntimeTexture metallicTexture;
        RuntimeTexture roughnessTexture;

        RuntimeShader shader;
    };
}

#endif // RUNTIME_MATERIAL_H
