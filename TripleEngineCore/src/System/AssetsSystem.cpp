#include "System/AssetsSystem.h"
#include "TLogger.h"
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TripleEngineCore::System {
    uint32_t AssetsSystem::loadModel(const std::string& path) {
        auto it = modelMap.find(path);
        if (it != modelMap.end()) return it->second;

        auto model = std::make_unique<Graphics::Model>(Graphics::Model::LoadOBJ(path));
        uint32_t index = static_cast<uint32_t>(models.size());
        models.push_back(std::move(model));
        modelMap[path] = index;
        return index;
    }

    uint32_t AssetsSystem::loadModel(const std::string& name, Graphics::Model&& model) {
        if (modelMap.find(name) != modelMap.end()) {
            return modelMap[name];
        }

        auto ptr = std::make_unique<Graphics::Model>(std::move(model));
        uint32_t index = static_cast<uint32_t>(models.size());
        models.push_back(std::move(ptr));
        modelMap[name] = index;
        return index;
    }

    const Graphics::Model* AssetsSystem::getModel(uint32_t index) const {
        if (index >= models.size()) return nullptr;
        return models[index].get();
    }

    uint32_t AssetsSystem::loadShader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string key = vertexPath + "|" + fragmentPath;
        auto it = shaderMap.find(key);
        if (it != shaderMap.end())
            return it->second;

        std::ifstream vertexFile(vertexPath);
        if (!vertexFile.is_open()) {
			TripleLogger::TLogger::ModuleError("AssetsSystem", "Failed to open vertex shader: {}", vertexPath);
            return UINT32_MAX;
        }
        std::stringstream vertexStream;
        vertexStream << vertexFile.rdbuf();
        std::string vertexSource = vertexStream.str();
        vertexFile.close();

        std::ifstream fragmentFile(fragmentPath);
        if (!fragmentFile.is_open()) {
			TripleLogger::TLogger::ModuleError("AssetsSystem", "Failed to open fragment shader: {}", fragmentPath);
            return UINT32_MAX;
        }
        std::stringstream fragmentStream;
        fragmentStream << fragmentFile.rdbuf();
        std::string fragmentSource = fragmentStream.str();
        fragmentFile.close();

        auto shader = std::make_unique<Graphics::Shader>();
        shader->vertexPath = vertexPath;
        shader->fragmentPath = fragmentPath;
        shader->vertexSource = std::move(vertexSource);
        shader->fragmentSource = std::move(fragmentSource);

        uint32_t index = static_cast<uint32_t>(shaders.size());
        shaders.push_back(std::move(shader));
        shaderMap[key] = index;

        return index;
    }

    const Graphics::Shader* AssetsSystem::getShader(uint32_t index) const {
        if (index >= shaders.size()) return nullptr;
        return shaders[index].get();
    }

    uint32_t AssetsSystem::createMaterial(const Graphics::Material& mat, const std::string& name = "") {
        if (!name.empty() && materialMap.find(name) != materialMap.end()) {
            return materialMap[name];
        }

        auto material = std::make_unique<Graphics::Material>(mat);
        uint32_t index = static_cast<uint32_t>(materials.size());
        materials.push_back(std::move(material));

        if (!name.empty()) materialMap[name] = index;
        return index;
    }

    const Graphics::Material* AssetsSystem::getMaterial(uint32_t index) const {
        if (index >= materials.size()) return nullptr;
        return materials[index].get();
    }

    uint32_t AssetsSystem::loadTexture(const std::string& path) {
        auto it = textureMap.find(path);
        if (it != textureMap.end())
            return it->second;

        int width, height, channels_in_file;
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels_in_file, 4); // RGBA
        if (!pixels) {
			TripleLogger::TLogger::ModuleError("AssetsSystem", "Failed to load texture: {}", path);
            return UINT32_MAX;
        }

        auto texture = std::make_unique<Graphics::Texture>();
        texture->path = path;
        texture->width = static_cast<uint16_t>(width);
        texture->height = static_cast<uint16_t>(height);
        texture->data.assign(pixels, pixels + width * height * 4);

        stbi_image_free(pixels);

        uint32_t index = static_cast<uint32_t>(textures.size());
        textures.push_back(std::move(texture));
        textureMap[path] = index;

        return index;
    }

    const Graphics::Texture* AssetsSystem::getTexture(uint32_t index) const {
        if (index >= textures.size()) return nullptr;
        return textures[index].get();
    }
}