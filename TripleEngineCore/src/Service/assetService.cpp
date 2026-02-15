#include "Service/AssetService.h"
#include "TLogger.h"
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include "Utils/AssimpHelper.h"
#include "Asset/AssetStorage.h"

using namespace TripleEngineCore::Asset;

namespace TripleEngineCore::Service {
    Service::MaterialID processMaterial(const  Utils::AssimpHelper::LoadedMesh& _mesh,
        const Utils::AssimpHelper::LoadedModel& loadedModel,
        AssetService* system)
    {
        Utils::AssimpHelper::LoadedMaterial _mat = loadedModel.materials[_mesh.materialIndex];
        Utils::AssimpHelper::LoadedTexture _difftex;
        bool isEmpty = true;
        if (!_mat.diffuseTextures.empty()) {
            _difftex = _mat.diffuseTextures[0];
            isEmpty = false;
        }
        Asset::Texture texture;
        texture.name = _difftex.name;
        texture.width = static_cast<uint16_t>(_difftex.width);
        texture.height = static_cast<uint16_t>(_difftex.height);
        texture.channels = static_cast<uint8_t>(_difftex.channels);
        texture.pixels = std::move(_difftex.pixels);

        Asset::Material material;
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

    void processMeshes(Asset::Model* model, const Utils::AssimpHelper::LoadedModel& loadedModel, AssetService* system) {
        for (const auto& _mesh : loadedModel.meshes) {
            Asset::Mesh mesh;
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

namespace TripleEngineCore::Service {

    struct AssetService::Impl
    {
        Asset::AssetStorage<Asset::Model> models;
        Asset::AssetStorage<Asset::Shader> shaders;
        Asset::AssetStorage<Asset::Material> materials;
        Asset::AssetStorage<Asset::Texture> textures;

        std::function<void(const Asset::Texture*)> _onTextureLoaded;
        std::function<void(const Asset::Model*)>  _onModelLoaded;
        std::function<void(const Asset::Shader*)> _onShaderLoaded;
    };

    AssetService::AssetService() : _impl(new Impl()) {}
	AssetService::~AssetService() { delete _impl; }

    ModelID AssetService::loadModelFromFile(const std::string& name, const std::string& path)
    {
        if (_impl->models.exists(name))
            return _impl->models.getID(name);

        Utils::AssimpHelper::LoadedModel loadedModel = Utils::AssimpHelper::LoadModel(path);
        if (loadedModel.meshes.size() <= 0) {
            TripleLogger::TLogger::ModuleWarn("AssetService", "(model: {}) the model has no meshes and as a result was not loaded", name);
            return INVALID_ASSET_ID;
        }

        auto engineModel = std::make_unique<Asset::Model>();
        engineModel->meshes.reserve(loadedModel.meshes.size());
		processMeshes(engineModel.get(), loadedModel, this);

		auto ptr = engineModel.get();
        Asset::AssetID id = _impl->models.add(name, std::move(engineModel));

        if (_impl->_onModelLoaded) {
            _impl->_onModelLoaded(ptr);
        }

        return id;
    }

    ModelID AssetService::loadModelFromModel(const std::string& name, Model&& model) {
        if (_impl->models.exists(name))
            return _impl->models.getID(name);

        auto _model = std::make_unique<Model>(std::move(model));

		auto ptr = _model.get();
        Asset::AssetID id = _impl->models.add(name, std::move(_model));

        if (_impl->_onModelLoaded) {
            _impl->_onModelLoaded(ptr);
        }

        return id;
    }

    ModelID AssetService::getModelId(const std::string& name) const {
        return _impl->models.getID(name);
    }

    const Model* AssetService::getModel(ModelID id) const {
        return _impl->models.get(id);
    }

    Asset::Model* AssetService::getModelMutable(ModelID id)
    {
        return _impl->models.getMutable(id);
    }

    ShaderID AssetService::loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        if (_impl->shaders.exists(name))
            return _impl->shaders.getID(name);

        std::ifstream vsFile(vertexPath);
        std::ifstream fsFile(fragmentPath);
        if (!vsFile.is_open() || !fsFile.is_open()) {
            TripleLogger::TLogger::ModuleError("AssetService", "Failed to open shader '{}'", name);
            return INVALID_ASSET_ID;
        }

        std::stringstream vsStream, fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();

        auto shader = std::make_unique<Shader>();
        shader->vertexSource = vsStream.str();
        shader->fragmentSource = fsStream.str();

		auto ptr = shader.get();
        Asset::AssetID id = _impl->shaders.add(name, std::move(shader));

        if(_impl->_onShaderLoaded) {
            _impl->_onShaderLoaded(ptr);
		}

        return id;
    }

    ShaderID AssetService::getShaderId(const std::string& name) const {
        return _impl->shaders.getID(name);
    }

    const Shader* AssetService::getShader(ShaderID id) const {
        return _impl->shaders.get(id);
    }

    MaterialID AssetService::createMaterial(const std::string& name, const Material& material) {
        if (_impl->materials.exists(name))
            return _impl->materials.getID(name);

        auto ptr = std::make_unique<Material>(material);
        return _impl->materials.add(name, std::move(ptr));
    }

    MaterialID AssetService::getMaterialId(const std::string& name) const {
        return _impl->materials.getID(name);
    }

    const Material* AssetService::getMaterial(MaterialID id) const {
        return _impl->materials.get(id);
    }

    TextureID AssetService::loadTexture(const std::string& name, Asset::Texture&& texture)
    {
        if (_impl->textures.exists(name))
            return _impl->textures.getID(name);

        if (texture.pixels.empty()) {
            TripleLogger::TLogger::ModuleError("AssetService", "The {} texture is empty", name);
            return INVALID_ASSET_ID;
        }

		auto _texture = std::make_unique<Asset::Texture>(std::move(texture));
        auto ptr = _texture.get();
        Asset::AssetID id = _impl->textures.add(name, std::move(_texture));

        if (_impl->_onTextureLoaded) {
            _impl->_onTextureLoaded(ptr);
        }

		return id;
    }

    TextureID AssetService::loadTextureFromFile(const std::string& name, const std::string& path) {
        if (_impl->textures.exists(name))
            return _impl->textures.getID(name);

        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!pixels) {
            TripleLogger::TLogger::ModuleError("AssetService", "Failed to load texture '{}': {}", name, path);
            return INVALID_ASSET_ID;
        }

        auto texture = std::make_unique<Texture>();
        texture->width = static_cast<uint16_t>(width);
        texture->height = static_cast<uint16_t>(height);
        texture->channels = 4;
        texture->pixels.assign(pixels, pixels + width * height * texture->channels);

        stbi_image_free(pixels);

		auto ptr = texture.get();
        Asset::AssetID id = _impl->textures.add(name, std::move(texture));

        if (_impl->_onTextureLoaded) {
            _impl->_onTextureLoaded(ptr);
		}

        return id;
    }

    TextureID AssetService::getTextureId(const std::string& name) const {
        return _impl->textures.getID(name);
    }

    TextureID AssetService::genSolidTexture(
        const std::string& name,
        uint8_t r, uint8_t g, uint8_t b, uint8_t a
    ) {
        if (_impl->textures.exists(name))
            return _impl->textures.getID(name);

        auto tex = std::make_unique<Asset::Texture>();
        tex->width = 1;
        tex->height = 1;
        tex->channels = 4;
        tex->pixels = { r, g, b, a };

		auto ptr = tex.get();
        Asset::AssetID id = _impl->textures.add(name, std::move(tex));

        if (_impl->_onTextureLoaded) {
            _impl->_onTextureLoaded(ptr);
        }

        return id;
    }

    const Texture* AssetService::getTexture(TextureID id) const {
        return _impl->textures.get(id);
    }

    bool AssetService::loadDefaultAssets()
    {
        using namespace TripleMath;
        Service::TextureID ard = this->genSolidTexture(DefaultAlbedoRoughnessName, 255, 255, 255, 255); // albedo, roughness
        Service::TextureID mtd = this->genSolidTexture(DefaultMetallicName, 0, 0, 0, 255); // metallic
        Service::TextureID nd = this->genSolidTexture(DefaultNormalName, 128, 128, 255, 255); // normal

        Service::ShaderID sd = this->loadShaderFromFile(DefaultShaderName,
            "assets\\shaders\\__default_shader.vert",
            "assets\\shaders\\__default_shader.frag");

        if (ard == Asset::INVALID_ASSET_ID ||
            mtd == Asset::INVALID_ASSET_ID ||
            nd == Asset::INVALID_ASSET_ID ||
            sd == Asset::INVALID_ASSET_ID) 
        {
            TripleLogger::TLogger::ModuleCritical("AssetService", "The default resources were not loaded properly, and the program cannot continue working normally.");
            return false;
        }

        Asset::Material mtdd;
        mtdd.albedoColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        mtdd.albedoTextureId = ard;
        mtdd.metallicTextureId = mtd;
        mtdd.normalTextureId = nd;
        mtdd.roughnessTextureId = ard;
        mtdd.shaderId = sd;
        mtdd.metallic = 0.1;
        mtdd.roughness = 1.0;
        Service::MaterialID mdid = this->createMaterial(DefaultMaterialName, mtdd);
        if (mdid == Asset::INVALID_ASSET_ID) {
            TripleLogger::TLogger::ModuleCritical("AssetService", "The default resources were not loaded properly, and the program cannot continue working normally.");
            return false;
        }

        return true;
    }

    void AssetService::setTextureLoadedCallback(std::function<void(const Asset::Texture*)> cb)
    {
        _impl->_onTextureLoaded = std::move(cb);
    }

    void AssetService::setModelLoadedCallback(std::function<void(const Asset::Model*)> cb)
    {
        _impl->_onModelLoaded = std::move(cb);
    }

    void AssetService::setShaderLoadedCallback(std::function<void(const Asset::Shader*)> cb)
    {
        _impl->_onShaderLoaded = std::move(cb);
    }
}