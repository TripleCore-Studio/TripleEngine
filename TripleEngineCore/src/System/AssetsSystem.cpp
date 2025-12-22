#include "System/AssetsSystem.h"
#include "TLogger.h"
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace TripleEngineCore::Asset;

namespace TripleEngineCore::System {
    ModelID AssetsSystem::loadModelFromFile(const std::string& name, const std::string& path) {
        if (models.exists(name))
            return models.getID(name);

		Model model = Model::CreateCube(); // Placeholder for actual model loading logic
        for(auto& mesh : model.meshes) {
			mesh.computeHash();
		}
        auto ptr = std::make_unique<Model>(std::move(model));
        return models.add(name, std::move(ptr));
    }

    ModelID AssetsSystem::loadModelFromModel(const std::string& name, Model&& model) {
        if (models.exists(name))
            return models.getID(name);

        for (auto& mesh : model.meshes) {
            mesh.computeHash();
        }

        auto ptr = std::make_unique<Model>(std::move(model));
        return models.add(name, std::move(ptr));
    }

    ModelID AssetsSystem::getModelIndex(const std::string& name) const {
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
		shader->computeHash();

        return shaders.add(name, std::move(shader));
    }

    ShaderID AssetsSystem::getShaderIndex(const std::string& name) const {
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

    MaterialID AssetsSystem::getMaterialIndex(const std::string& name) const {
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
		texture->computeHash();

        stbi_image_free(pixels);

        return textures.add(name, std::move(texture));
    }

    TextureID AssetsSystem::getTextureIndex(const std::string& name) const {
        return textures.getID(name);
    }

    const Texture* AssetsSystem::getTexture(TextureID id) const {
        return textures.get(id);
    }
}