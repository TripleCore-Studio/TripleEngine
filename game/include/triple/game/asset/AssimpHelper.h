#ifndef ASSIMP_HELPER
#define ASSIMP_HELPER

#include <vector>
#include <string>
#include <triple/gfx/Vertex.h>
#include <triple/math/Vec4.h>

#include "triple/game/asset/Material.h"

struct aiScene;
struct aiMaterial;
struct aiTexture;

namespace triple::game {
	class AssimpHelper {
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
			math::Vec4 diffuseColor{1.0f, 1.0f, 1.0f, 1.0f};
			float opacity = 1.0f;
			MaterialBlendMode blendMode = MaterialBlendMode::Opaque;
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

		static LoadedModel loadModel(const std::string &path);
		~AssimpHelper() = default;

	private:
		static LoadedTexture loadEmbeddedTexture(const ::aiTexture *texture);
		static LoadedTexture loadFileTexture(const std::string &path);
		static LoadedTexture loadTexture(const ::aiMaterial *material, uint16_t type,
		                                 const ::aiScene *scene);
		static void loadGeometry(const ::aiScene *scene, LoadedModel &outModel);
		static void loadMaterials(const ::aiScene *scene, LoadedModel &outModel);
	};
} // namespace triple::game

#endif // ASSIMP_HELPER
