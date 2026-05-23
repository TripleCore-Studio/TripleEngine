#ifndef ASSIMP_HELPER
#define ASSIMP_HELPER

#include <vector>
#include <string>
#include <triple/gfx/Vertex.h>

struct aiScene;
struct aiMaterial;
struct aiTexture;

namespace triple::core {
    class AssimpHelper
    {
    public:
        struct LoadedTexture {
            std::string name;
            int width, height, channels;
            std::vector<unsigned char> pixels;
        };

        struct LoadedMaterial {
            std::string name;
            std::vector<LoadedTexture> diffuseTextures;
            std::vector<LoadedTexture> normalTextures;
        };

        struct LoadedMesh {
            std::string name;
            std::vector<gfx::Vertex> vertices;
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
        static LoadedTexture LoadEmbeddedTexture(const ::aiTexture* texture);
        static LoadedTexture LoadFileTexture(const std::string& path);
        static LoadedTexture LoadTexture(const ::aiMaterial* material, uint16_t type, const ::aiScene* scene);
		static void LoadGeometry(const ::aiScene* scene, LoadedModel& outModel);
		static void LoadMaterials(const ::aiScene* scene, LoadedModel& outModel);
    };
}

#endif // ASSIMP_HELPER
