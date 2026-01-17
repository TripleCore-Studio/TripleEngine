#ifndef ASSETS_SYSTEM_H
#define ASSETS_SYSTEM_H

#include <string>

#include "Interfaces/IServiceSystem.h"
#include "Asset/AssetStorage.h"

#include "Asset/Asset.h"
#include "Asset/Model.h"
#include "Asset/Shader.h"
#include "Asset/Material.h"
#include "Asset/Texture.h"

namespace TripleEngineCore::System {
    using ModelID = Asset::AssetID;
    using ShaderID = Asset::AssetID;
    using MaterialID = Asset::AssetID;
    using TextureID = Asset::AssetID;

    class AssetsSystem : public IServiceSystem {
    public:
        AssetsSystem() = default;
        ~AssetsSystem() = default;

        virtual void init() {}
        virtual void shutdown() {}

        ModelID loadModelFromFile(const std::string& name, const std::string& path);
        ModelID loadModelFromModel(const std::string& name, Asset::Model&& model);
        ModelID getModelId(const std::string& name) const;
        const Asset::Model* getModel(ModelID id) const;
        Asset::Model* getModelMutable(ModelID id);

        ShaderID loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
        ShaderID getShaderId(const std::string& name) const;
        const Asset::Shader* getShader(ShaderID id) const;

        MaterialID createMaterial(const std::string& name, const Asset::Material& material);
        MaterialID getMaterialId(const std::string& name) const;
        const Asset::Material* getMaterial(MaterialID id) const;

        TextureID loadTexture(const std::string& name, const std::string& path);
        TextureID getTextureId(const std::string& name) const;
        TextureID genSolidTexture(const std::string& name, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        const Asset::Texture* getTexture(TextureID id) const;

    private:
        Asset::AssetStorage<Asset::Model> models;
        Asset::AssetStorage<Asset::Shader> shaders;
        Asset::AssetStorage<Asset::Material> materials;
        Asset::AssetStorage<Asset::Texture> textures;
    };
}

#endif // ASSETS_SYSTEM_H
