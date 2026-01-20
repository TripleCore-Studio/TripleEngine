#include "System/AssetsSystem.h"
#include "TLogger.h"
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include "Utils/AssimpHelper.h"
#include "Asset/AssetStorage.h"

using namespace TripleEngineCore::Asset;

namespace TripleEngineCore::System {

    struct AssetsSystem::Impl
    {
        Asset::AssetStorage<Asset::Model> models;
        Asset::AssetStorage<Asset::Shader> shaders;
        Asset::AssetStorage<Asset::Material> materials;
        Asset::AssetStorage<Asset::Texture> textures;

        std::function<void(const Asset::Texture*)> _onTextureLoaded;
        std::function<void(const Asset::Model*)>  _onModelLoaded;
        std::function<void(const Asset::Shader*)> _onShaderLoaded;
    };


    AssetsSystem::AssetsSystem() : _impl(new Impl()) {}
	AssetsSystem::~AssetsSystem() { delete _impl; }

    ModelID AssetsSystem::loadModelFromFile(const std::string& name, const std::string& path)
    {
        if (_impl->models.exists(name))
            return _impl->models.getID(name);

        Utils::AssimpHelper::LoadedModel model = Utils::AssimpHelper::LoadModel(path);
        if (model.meshes.size() <= 0) {
            TripleLogger::TLogger::ModuleWarn("AssetsSystem", "(model: {}) the model has no meshes and as a result was not loaded", name);
            return INVALID_ASSET_ID;
        }

        auto engineModel = std::make_unique<Asset::Model>();
        engineModel->meshes.reserve(model.meshes.size());

        for (const auto& m : model.meshes) {
            Asset::Mesh mesh;
            mesh.name = m.name;

            uint32_t baseVertex = engineModel->vertices.size();
            uint32_t baseIndex = engineModel->indices.size();

            engineModel->vertices.insert(
                engineModel->vertices.end(),
                m.vertices.begin(),
                m.vertices.end()
            );

            for (uint32_t idx : m.indices)
                engineModel->indices.push_back(idx + baseVertex);

            Primitive prim;
            prim.indexOffset = baseIndex;
            prim.indexCount = m.indices.size();
            mesh.primitives.push_back(std::move(prim));
            engineModel->meshes.push_back(std::move(mesh));
        }

		auto ptr = engineModel.get();
        Asset::AssetID id = _impl->models.add(name, std::move(engineModel));

        if (_impl->_onModelLoaded) {
            _impl->_onModelLoaded(ptr);
        }

        return id;
    }

    ModelID AssetsSystem::loadModelFromModel(const std::string& name, Model&& model) {
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

    ModelID AssetsSystem::getModelId(const std::string& name) const {
        return _impl->models.getID(name);
    }

    const Model* AssetsSystem::getModel(ModelID id) const {
        return _impl->models.get(id);
    }

    Asset::Model* AssetsSystem::getModelMutable(ModelID id)
    {
        return _impl->models.getMutable(id);
    }

    ShaderID AssetsSystem::loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        if (_impl->shaders.exists(name))
            return _impl->shaders.getID(name);

        std::ifstream vsFile(vertexPath);
        std::ifstream fsFile(fragmentPath);
        if (!vsFile.is_open() || !fsFile.is_open()) {
            TripleLogger::TLogger::ModuleError("AssetsSystem", "Failed to open shader '{}'", name);
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

    ShaderID AssetsSystem::getShaderId(const std::string& name) const {
        return _impl->shaders.getID(name);
    }

    const Shader* AssetsSystem::getShader(ShaderID id) const {
        return _impl->shaders.get(id);
    }

    MaterialID AssetsSystem::createMaterial(const std::string& name, const Material& material) {
        if (_impl->materials.exists(name))
            return _impl->materials.getID(name);

        auto ptr = std::make_unique<Material>(material);
        return _impl->materials.add(name, std::move(ptr));
    }

    MaterialID AssetsSystem::getMaterialId(const std::string& name) const {
        return _impl->materials.getID(name);
    }

    const Material* AssetsSystem::getMaterial(MaterialID id) const {
        return _impl->materials.get(id);
    }

    TextureID AssetsSystem::loadTexture(const std::string& name, const std::string& path) {
        if (_impl->textures.exists(name))
            return _impl->textures.getID(name);

        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!pixels) {
            TripleLogger::TLogger::ModuleError("AssetsSystem", "Failed to load texture '{}': {}", name, path);
            return INVALID_ASSET_ID;
        }

        auto texture = std::make_unique<Texture>();
        texture->width = static_cast<uint16_t>(width);
        texture->height = static_cast<uint16_t>(height);
        texture->channels = 4;
        texture->data.assign(pixels, pixels + width * height * texture->channels);

        stbi_image_free(pixels);

		auto ptr = texture.get();
        Asset::AssetID id = _impl->textures.add(name, std::move(texture));

        if (_impl->_onTextureLoaded) {
            _impl->_onTextureLoaded(ptr);
		}

        return id;
    }

    TextureID AssetsSystem::getTextureId(const std::string& name) const {
        return _impl->textures.getID(name);
    }

    TextureID AssetsSystem::genSolidTexture(
        const std::string& name,
        uint8_t r, uint8_t g, uint8_t b, uint8_t a
    ) {
        if (_impl->textures.exists(name))
            return _impl->textures.getID(name);

        auto tex = std::make_unique<Asset::Texture>();
        tex->width = 1;
        tex->height = 1;
        tex->channels = 4;
        tex->data = { r, g, b, a };

		auto ptr = tex.get();
        Asset::AssetID id = _impl->textures.add(name, std::move(tex));

        if (_impl->_onTextureLoaded) {
            _impl->_onTextureLoaded(ptr);
        }

        return id;
    }

    const Texture* AssetsSystem::getTexture(TextureID id) const {
        return _impl->textures.get(id);
    }

    void AssetsSystem::setTextureLoadedCallback(std::function<void(const Asset::Texture*)> cb)
    {
        _impl->_onTextureLoaded = std::move(cb);
    }

    void AssetsSystem::setModelLoadedCallback(std::function<void(const Asset::Model*)> cb)
    {
        _impl->_onModelLoaded = std::move(cb);
    }

    void AssetsSystem::setShaderLoadedCallback(std::function<void(const Asset::Shader*)> cb)
    {
        _impl->_onShaderLoaded = std::move(cb);
    }
}