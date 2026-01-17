#ifndef ASSIMP_HELPER
#define ASSIMP_HELPER

#include <vector>
#include <string>
#include "Graphics/Vertex.h"

struct aiScene;
struct aiMaterial;
struct aiTexture;

namespace TripleEngineCore::Utils {
    class AssimpHelper
    {
    public:
        struct LoadedTexture {
            std::string name;
            int width, height;
            std::vector<unsigned char> pixels;
        };

        struct LoadedMaterial {
            std::string name;
            std::vector<LoadedTexture> diffuseTextures;
            std::vector<LoadedTexture> normalTextures;
        };

        struct LoadedMesh {
            std::string name;
            std::vector<Graphics::Vertex> vertices;
            std::vector<uint32_t> indices;
            uint32_t materialIndex = 0;
        };

        struct LoadedModel {
            std::vector<LoadedMesh> meshes;
            std::vector<LoadedMaterial> materials;
        };

        static LoadedModel LoadModel(const std::string& path);
        ~AssimpHelper() = default;
    private:
        static LoadedTexture LoadEmbeddedTexture(::aiTexture* texture);
        static LoadedTexture LoadFileTexture(std::string path);
        static LoadedTexture LoadTexture(::aiMaterial* material, uint16_t type, const ::aiScene* scene);
    };
}

#endif // ASSIMP_HELPER
