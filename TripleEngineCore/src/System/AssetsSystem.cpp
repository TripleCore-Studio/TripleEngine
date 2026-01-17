#include "System/AssetsSystem.h"
#include "TLogger.h"
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include "Utils/AssimpHelper.h"

using namespace TripleEngineCore::Asset;

namespace TripleEngineCore::System {
    ModelID AssetsSystem::loadModelFromFile(const std::string& name, const std::string& path)
    {
        if (models.exists(name))
            return models.getID(name);

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

        return models.add(name, std::move(engineModel));
    }

    ModelID AssetsSystem::loadModelFromModel(const std::string& name, Model&& model) {
        if (models.exists(name))
            return models.getID(name);

        auto ptr = std::make_unique<Model>(std::move(model));
        return models.add(name, std::move(ptr));
    }

    ModelID AssetsSystem::getModelId(const std::string& name) const {
        return models.getID(name);
    }

    const Model* AssetsSystem::getModel(ModelID id) const {
        return models.get(id);
    }

    Asset::Model* AssetsSystem::getModelMutable(ModelID id)
    {
        return models.getMutable(id);
    }

    ShaderID AssetsSystem::loadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
        if (shaders.exists(name))
            return shaders.getID(name);

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

        return shaders.add(name, std::move(shader));
    }

    ShaderID AssetsSystem::getShaderId(const std::string& name) const {
        return shaders.getID(name);
    }

    const Shader* AssetsSystem::getShader(ShaderID id) const {
        return shaders.get(id);
    }

    MaterialID AssetsSystem::createMaterial(const std::string& name, const Material& material) {
        if (materials.exists(name))
            return materials.getID(name);

        auto ptr = std::make_unique<Material>(material);
        return materials.add(name, std::move(ptr));
    }

    MaterialID AssetsSystem::getMaterialId(const std::string& name) const {
        return materials.getID(name);
    }

    const Material* AssetsSystem::getMaterial(MaterialID id) const {
        return materials.get(id);
    }

    TextureID AssetsSystem::loadTexture(const std::string& name, const std::string& path) {
        if (textures.exists(name))
            return textures.getID(name);

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

        return textures.add(name, std::move(texture));
    }

    TextureID AssetsSystem::getTextureId(const std::string& name) const {
        return textures.getID(name);
    }

    TextureID AssetsSystem::genSolidTexture(
        const std::string& name,
        uint8_t r, uint8_t g, uint8_t b, uint8_t a
    ) {
        if (textures.exists(name))
            return textures.getID(name);

        auto tex = std::make_unique<Asset::Texture>();
        tex->width = 1;
        tex->height = 1;
        tex->channels = 4;
        tex->data = { r, g, b, a };

        return textures.add(name, std::move(tex));
    }

    const Texture* AssetsSystem::getTexture(TextureID id) const {
        return textures.get(id);
    }
}