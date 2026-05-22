#include "triple/core/Service/AssetService.h"
#include <triple/log/Logger.h>
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include "triple/core/Utils/AssimpHelper.h"
#include "triple/core/Asset/AssetStorage.h"

namespace triple::core {
    MaterialID processMaterial(const AssimpHelper::LoadedMesh& _mesh,
        const AssimpHelper::LoadedModel& loadedModel,
        AssetService* system)
    {
        AssimpHelper::LoadedMaterial _mat = loadedModel.materials[_mesh.materialIndex];
        AssimpHelper::LoadedTexture _difftex;
        bool isEmpty = true;
        if (!_mat.diffuseTextures.empty()) {
            _difftex = _mat.diffuseTextures[0];
            isEmpty = false;
        }
        Texture texture;
        texture.name = _difftex.name;
        texture.width = static_cast<uint16_t>(_difftex.width);
        texture.height = static_cast<uint16_t>(_difftex.height);
        texture.channels = static_cast<uint8_t>(_difftex.channels);
        texture.pixels = std::move(_difftex.pixels);

        Material material;
        material.name = _mat.name;
        if (!isEmpty) {
            material.albedoTextureId = system->loadTexture(material.name, std::move(texture));
        }
        else {
            material.albedoTextureId = system->getTextureId(DefaultAlbedoRoughnessName);
        }
        material.albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        material.metallic = 0.0f;
        material.roughness = 1.0f;
        material.metallicTextureId = system->getTextureId(DefaultMetallicName);
        material.normalTextureId = system->getTextureId(DefaultNormalName);
        material.roughnessTextureId = system->getTextureId(DefaultAlbedoRoughnessName);
        material.shaderId = system->getShaderId(DefaultShaderName);

        return system->createMaterial(material.name, material);
    }

    void processMeshes(Model* model, const AssimpHelper::LoadedModel& loadedModel, AssetService* system) {
        for (const auto& _mesh : loadedModel.meshes) {
            Mesh mesh;
            mesh.name = _mesh.name;

            uint32_t baseVertex = model->vertices.size();
            uint32_t baseIndex = model->indices.size();

            model->vertices.insert(
                model->vertices.end(),
                _mesh.vertices.begin(),
                _mesh.vertices.end()
            );

            for (uint32_t idx : _mesh.indices)
                model->indices.push_back(idx + baseVertex);

            Primitive prim;
            prim.indexOffset = baseIndex;
            prim.indexCount = _mesh.indices.size();
            prim.materialId = processMaterial(_mesh, loadedModel, system);
            mesh.primitives.push_back(std::move(prim));

            model->meshes.push_back(std::move(mesh));
        }
    }
}

namespace triple::core {

    struct AssetService::Impl
    {
        AssetStorage<Model> m_models;
        AssetStorage<Shader> m_shaders;
        AssetStorage<Material> m_materials;
        AssetStorage<Texture> m_textures;

        std::function<void(const Texture*)> m_onTextureLoaded;
        std::function<void(const Model*)>  m_onModelLoaded;
        std::function<void(const Shader*)> m_onShaderLoaded;
    };

    AssetService::AssetService() : m_impl(new Impl()) {}
	AssetService::~AssetService() { delete m_impl; }

    ModelID AssetService::loadModelFromFile(const std::string& name, const std::string& path)
    {
        if (m_impl->m_models.exists(name))
            return m_impl->m_models.getID(name);

        AssimpHelper::LoadedModel loadedModel = AssimpHelper::LoadModel(path);
        if (loadedModel.meshes.size() <= 0) {
            triple::log::Logger::ModuleWarn("AssetService", "(model: {}) the model has no meshes and as a result was not loaded", name);
            return INVALID_ASSET_ID;
        }

        auto engineModel = std::make_unique<Model>();
        engineModel->meshes.reserve(loadedModel.meshes.size());
		processMeshes(engineModel.get(), loadedModel, this);

		auto ptr = engineModel.get();
        AssetID id = m_impl->m_models.add(name, std::move(engineModel));

        if (m_impl->m_onModelLoaded) {
            m_impl->m_onModelLoaded(ptr);
        }

        return id;
    }

    ModelID AssetService::loadModelFromModel(const std::string& name, Model&& model) {
        if (m_impl->m_models.exists(name))
            return m_impl->m_models.getID(name);

        auto _model = std::make_unique<Model>(std::move(model));

		auto ptr = _model.get();
        AssetID id = m_impl->m_models.add(name, std::move(_model));

        if (m_impl->m_onModelLoaded) {
            m_impl->m_onModelLoaded(ptr);
        }

        return id;
    }

    ModelID AssetService::getModelId(const std::string& name) const {
        return m_impl->m_models.getID(name);
    }

    const Model* AssetService::getModel(ModelID id) const {
        return m_impl->m_models.get(id);
    }

    Model* AssetService::getModelMutable(ModelID id)
    {
        return m_impl->m_models.getMutable(id);
    }

    ShaderID AssetService::loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        if (m_impl->m_shaders.exists(name))
            return m_impl->m_shaders.getID(name);

        std::ifstream vsFile(vertexPath);
        std::ifstream fsFile(fragmentPath);
        if (!vsFile.is_open() || !fsFile.is_open()) {
            triple::log::Logger::ModuleError("AssetService", "Failed to open shader '{}'", name);
            return INVALID_ASSET_ID;
        }

        std::stringstream vsStream, fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();

        auto shader = std::make_unique<Shader>();
        shader->vertexSource = vsStream.str();
        shader->fragmentSource = fsStream.str();

		auto ptr = shader.get();
        AssetID id = m_impl->m_shaders.add(name, std::move(shader));

        if(m_impl->m_onShaderLoaded) {
            m_impl->m_onShaderLoaded(ptr);
		}

        return id;
    }

    ShaderID AssetService::getShaderId(const std::string& name) const {
        return m_impl->m_shaders.getID(name);
    }

    const Shader* AssetService::getShader(ShaderID id) const {
        return m_impl->m_shaders.get(id);
    }

    MaterialID AssetService::createMaterial(const std::string& name, const Material& material) {
        if (m_impl->m_materials.exists(name))
            return m_impl->m_materials.getID(name);

        auto ptr = std::make_unique<Material>(material);
        return m_impl->m_materials.add(name, std::move(ptr));
    }

    MaterialID AssetService::getMaterialId(const std::string& name) const {
        return m_impl->m_materials.getID(name);
    }

    const Material* AssetService::getMaterial(MaterialID id) const {
        return m_impl->m_materials.get(id);
    }

    TextureID AssetService::loadTexture(const std::string& name, Texture&& texture)
    {
        if (m_impl->m_textures.exists(name))
            return m_impl->m_textures.getID(name);

        if (texture.pixels.empty()) {
            triple::log::Logger::ModuleError("AssetService", "The {} texture is empty", name);
            return INVALID_ASSET_ID;
        }

		auto _texture = std::make_unique<Texture>(std::move(texture));
        auto ptr = _texture.get();
        AssetID id = m_impl->m_textures.add(name, std::move(_texture));

        if (m_impl->m_onTextureLoaded) {
            m_impl->m_onTextureLoaded(ptr);
        }

		return id;
    }

    TextureID AssetService::loadTextureFromFile(const std::string& name, const std::string& path) {
        if (m_impl->m_textures.exists(name))
            return m_impl->m_textures.getID(name);

        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!pixels) {
            triple::log::Logger::ModuleError("AssetService", "Failed to load texture '{}': {}", name, path);
            return INVALID_ASSET_ID;
        }

        auto texture = std::make_unique<Texture>();
        texture->width = static_cast<uint16_t>(width);
        texture->height = static_cast<uint16_t>(height);
        texture->channels = 4;
        texture->pixels.assign(pixels, pixels + width * height * texture->channels);

        stbi_image_free(pixels);

		auto ptr = texture.get();
        AssetID id = m_impl->m_textures.add(name, std::move(texture));

        if (m_impl->m_onTextureLoaded) {
            m_impl->m_onTextureLoaded(ptr);
		}

        return id;
    }

    TextureID AssetService::getTextureId(const std::string& name) const {
        return m_impl->m_textures.getID(name);
    }

    TextureID AssetService::genSolidTexture(
        const std::string& name,
        uint8_t r, uint8_t g, uint8_t b, uint8_t a
    ) {
        if (m_impl->m_textures.exists(name))
            return m_impl->m_textures.getID(name);

        auto tex = std::make_unique<Texture>();
        tex->width = 1;
        tex->height = 1;
        tex->channels = 4;
        tex->pixels = { r, g, b, a };

		auto ptr = tex.get();
        AssetID id = m_impl->m_textures.add(name, std::move(tex));

        if (m_impl->m_onTextureLoaded) {
            m_impl->m_onTextureLoaded(ptr);
        }

        return id;
    }

    const Texture* AssetService::getTexture(TextureID id) const {
        return m_impl->m_textures.get(id);
    }

    bool AssetService::loadDefaultAssets()
    {
        TextureID ard = this->genSolidTexture(DefaultAlbedoRoughnessName, 255, 255, 255, 255); // albedo, roughness
        TextureID mtd = this->genSolidTexture(DefaultMetallicName, 0, 0, 0, 255); // metallic
        TextureID nd = this->genSolidTexture(DefaultNormalName, 128, 128, 255, 255); // normal

        ShaderID sd = this->loadShaderFromFile(DefaultShaderName,
            "assets\\shaders\\__default_shader.vert",
            "assets\\shaders\\__default_shader.frag");

        if (ard == INVALID_ASSET_ID ||
            mtd == INVALID_ASSET_ID ||
            nd == INVALID_ASSET_ID ||
            sd == INVALID_ASSET_ID) 
        {
            triple::log::Logger::ModuleCritical("AssetService", "The default resources were not loaded properly, and the program cannot continue working normally.");
            return false;
        }

        Material mtdd;
        mtdd.albedoColor = triple::math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        mtdd.albedoTextureId = ard;
        mtdd.metallicTextureId = mtd;
        mtdd.normalTextureId = nd;
        mtdd.roughnessTextureId = ard;
        mtdd.shaderId = sd;
        mtdd.metallic = 0.1;
        mtdd.roughness = 1.0;
        MaterialID mdid = this->createMaterial(DefaultMaterialName, mtdd);
        if (mdid == INVALID_ASSET_ID) {
            triple::log::Logger::ModuleCritical("AssetService", "The default resources were not loaded properly, and the program cannot continue working normally.");
            return false;
        }

        return true;
    }

    void AssetService::setTextureLoadedCallback(std::function<void(const Texture*)> cb)
    {
        m_impl->m_onTextureLoaded = std::move(cb);
    }

    void AssetService::setModelLoadedCallback(std::function<void(const Model*)> cb)
    {
        m_impl->m_onModelLoaded = std::move(cb);
    }

    void AssetService::setShaderLoadedCallback(std::function<void(const Shader*)> cb)
    {
        m_impl->m_onShaderLoaded = std::move(cb);
    }
}