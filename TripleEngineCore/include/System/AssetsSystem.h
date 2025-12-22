#ifndef ASSETS_SYSTEM_H
#define ASSETS_SYSTEM_H

#include "Interfaces/IServiceSystem.h"
#include <string>
#include "Asset/AssetStorage.h"

#include "Asset/Model.h"
#include "Asset/Shader.h"
#include "Asset/Material.h"
#include "Asset/Texture.h"

namespace TripleEngineCore::System {
    using AssetID = Asset::AssetID;
    using ModelID = AssetID;
    using ShaderID = AssetID;
    using MaterialID = AssetID;
    using TextureID = AssetID;
    inline constexpr AssetID INVALID_ASSET_ID = TripleEngineCore::INVALID_INDEX;

    class AssetsSystem : public IServiceSystem {
    public:
        AssetsSystem() = default;
        ~AssetsSystem() = default;

        virtual void init() {}
        virtual void shutdown() {}

        ModelID loadModelFromFile(const std::string& name, const std::string& path);
        ModelID loadModelFromModel(const std::string& name, Asset::Model&& model);
        ModelID getModelIndex(const std::string& name) const;
        const Asset::Model* getModel(ModelID id) const;
        Asset::Model* getModelMutable(ModelID id);

        ShaderID loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
        ShaderID getShaderIndex(const std::string& name) const;
        const Asset::Shader* getShader(ShaderID id) const;

        MaterialID createMaterial(const std::string& name, const Asset::Material& material);
        MaterialID getMaterialIndex(const std::string& name) const;
        const Asset::Material* getMaterial(MaterialID id) const;

        TextureID loadTexture(const std::string& name, const std::string& path);
        TextureID getTextureIndex(const std::string& name) const;
        const Asset::Texture* getTexture(TextureID id) const;

    private:
        Asset::AssetStorage<Asset::Model> models;
        Asset::AssetStorage<Asset::Shader> shaders;
        Asset::AssetStorage<Asset::Material> materials;
        Asset::AssetStorage<Asset::Texture> textures;
    };
}

#endif // ASSETS_SYSTEM_H
