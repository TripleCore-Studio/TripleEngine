#ifndef ASSET_SERVICE_H
#define ASSET_SERVICE_H

#include <string>
#include <functional>

#include "triple/core/base/ExportMacros.h"
#include "triple/core/base/IService.h"

#include "triple/core/Asset/Asset.h"
#include "triple/core/Asset/Model.h"
#include "triple/core/Asset/Shader.h"
#include "triple/core/Asset/Material.h"
#include "triple/core/Asset/Texture.h"

namespace triple::core {
    class Engine;
}

namespace triple::core {
    using ModelID = AssetID;
    using ShaderID = AssetID;
    using MaterialID = AssetID;
    using TextureID = AssetID;

	inline const std::string DefaultAlbedoRoughnessName = "__default_white";
    inline const std::string DefaultMetallicName = "__default_black";
    inline const std::string DefaultNormalName = "__default_normal";
	inline const std::string DefaultShaderName = "__default_shader";
	inline const std::string DefaultMaterialName = "__default_material";

    class CORE_API AssetService : public IService {
    public:
        friend class triple::core::Engine;

        AssetService();
        ~AssetService();

        virtual void init() {}
        virtual void shutdown() {}

        ModelID loadModelFromFile(const std::string& name, const std::string& path);
        ModelID loadModelFromModel(const std::string& name, Model&& model);
        ModelID getModelId(const std::string& name) const;
        const Model* getModel(ModelID id) const;
        Model* getModelMutable(ModelID id);

        ShaderID loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
        ShaderID getShaderId(const std::string& name) const;
        const Shader* getShader(ShaderID id) const;

        MaterialID createMaterial(const std::string& name, const Material& material);
        MaterialID getMaterialId(const std::string& name) const;
        const Material* getMaterial(MaterialID id) const;

        TextureID loadTexture(const std::string& name, Texture&& texture);
        TextureID loadTextureFromFile(const std::string& name, const std::string& path);
        TextureID getTextureId(const std::string& name) const;
        TextureID genSolidTexture(const std::string& name, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        const Texture* getTexture(TextureID id) const;

		bool loadDefaultAssets();

        AssetService(const AssetService&) = delete;
        AssetService& operator=(const AssetService&) = delete;

        AssetService(AssetService&&) = default;
        AssetService& operator=(AssetService&&) = default;
    private:
		struct Impl;
		Impl* m_impl;

        void setTextureLoadedCallback(std::function<void(const Texture*)> cb);
        void setModelLoadedCallback(std::function<void(const Model*)> cb);
        void setShaderLoadedCallback(std::function<void(const Shader*)> cb);
    };
}

#endif // ASSET_SERVICE_H
