#ifndef ASSETS_SYSTEM_H
#define ASSETS_SYSTEM_H

#include "Interfaces/IServiceSystem.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"

namespace TripleEngineCore::System {
    class AssetsSystem : public IServiceSystem {
    public:
        AssetsSystem() = default;
        ~AssetsSystem() = default;

        virtual void init() {}

        virtual void shutdown() {}

        uint32_t loadModel(const std::string& path);

        uint32_t loadModel(const std::string& name, Graphics::Model&& model);

        const Graphics::Model* getModel(uint32_t index) const;

        uint32_t loadShader(const std::string& vertexPath, const std::string& fragmentPath);

        const Graphics::Shader* getShader(uint32_t index) const;

        uint32_t createMaterial(const Graphics::Material& mat, const std::string& name);

        const Graphics::Material* getMaterial(uint32_t index) const;

        uint32_t loadTexture(const std::string& path);

        const Graphics::Texture* getTexture(uint32_t index) const;

    private:
        std::vector<std::unique_ptr<Graphics::Model>> models;
        std::vector<std::unique_ptr<Graphics::Shader>> shaders;
        std::vector<std::unique_ptr<Graphics::Material>> materials;
        std::vector<std::unique_ptr<Graphics::Texture>> textures;

        std::unordered_map<std::string, uint32_t> modelMap;
        std::unordered_map<std::string, uint32_t> shaderMap;
        std::unordered_map<std::string, uint32_t> materialMap;
        std::unordered_map<std::string, uint32_t> textureMap;
    };
}

#endif // ASSETS_SYSTEM_H
