#include "triple/game/asset/ModelLoader.h"

#include <assert.h>

#include <triple/log/Logger.h>

#include "triple/game/asset/AssimpHelper.h"
#include "triple/game/asset/AssetManager.h"
#include "triple/game/asset/DefaultAssets.h"

namespace triple::game {
	template <typename T>
	TypedAssetID<T> getDefaultOrAssert(AssetManager *assetManager, std::string_view name) {
		auto id = assetManager->storageFor<T>().findByName(std::string(name));
		assert(id.isValid() && "Default asset not registered at startup");
		return id;
	}

	TypedAssetID<Material> processMaterial(const AssimpHelper::LoadedMesh &mesh,
	                                       const AssimpHelper::LoadedModel &loadedModel,
	                                       AssetManager *manager) {
		AssimpHelper::LoadedMaterial mat = loadedModel.materials[mesh.materialIndex];
		AssimpHelper::LoadedTexture difftex;
		bool isEmpty = true;
		if (!mat.diffuseTextures.empty()) {
			difftex = mat.diffuseTextures[0];
			isEmpty = false;
		}
		Texture texture;
		texture.width = static_cast<uint16_t>(difftex.width);
		texture.height = static_cast<uint16_t>(difftex.height);
		texture.channels = static_cast<uint8_t>(difftex.channels);
		texture.pixels = std::move(difftex.pixels);

		Material material;
		if (!isEmpty) {
			material.albedoTexture =
			    manager->create<Texture>(mat.name + difftex.name, std::move(texture));
		} else {
			material.albedoTexture =
			    getDefaultOrAssert<Texture>(manager, kDefaultAlbedoTextureName);
		}

		// TODO: model loading is currently superficial — material params (albedoColor,
		// metallic, roughness) and metallic/normal/roughness textures are not read from
		// the Assimp material at all; always falls back to hardcoded defaults.
		material.albedoColor = {1.0f, 1.0f, 1.0f, 1.0f};
		material.metallic = 0.0f;
		material.roughness = 1.0f;
		material.metallicTexture =
		    getDefaultOrAssert<Texture>(manager, kDefaultMetallicTextureName);
		material.normalTexture = getDefaultOrAssert<Texture>(manager, kDefaultNormalTextureName);
		material.roughnessTexture =
		    getDefaultOrAssert<Texture>(manager, kDefaultRoughnessTextureName);
		material.shader = getDefaultOrAssert<Shader>(manager, kDefaultShaderName);

		return manager->create<Material>(mat.name, material);
	}

	void processMeshes(Model &model, const AssimpHelper::LoadedModel &loadedModel,
	                   AssetManager *manager) {
		for (const auto &mesh : loadedModel.meshes) {
			Mesh eMesh;
			eMesh.name = mesh.name;

			uint32_t baseVertex = static_cast<uint32_t>(model.vertices.size());
			uint32_t baseIndex = static_cast<uint32_t>(model.indices.size());

			model.vertices.insert(model.vertices.end(), mesh.vertices.begin(), mesh.vertices.end());

			for (auto &vert : mesh.vertices) {
				model.boundsMin.x = std::min(model.boundsMin.x, vert.position.x);
				model.boundsMin.y = std::min(model.boundsMin.y, vert.position.y);
				model.boundsMin.z = std::min(model.boundsMin.z, vert.position.z);

				model.boundsMax.x = std::max(model.boundsMax.x, vert.position.x);
				model.boundsMax.y = std::max(model.boundsMax.y, vert.position.y);
				model.boundsMax.z = std::max(model.boundsMax.z, vert.position.z);
			}

			for (uint32_t idx : mesh.indices) {
				model.indices.push_back(idx + baseVertex);
			}

			Primitive prim;
			prim.indexOffset = baseIndex;
			prim.indexCount = static_cast<uint32_t>(mesh.indices.size());
			prim.material = processMaterial(mesh, loadedModel, manager);
			eMesh.primitives.push_back(prim);

			model.meshes.push_back(std::move(eMesh));
		}
	}

	std::optional<Model> ModelLoader::load(const ModelLoadParams &params) {
		AssimpHelper::LoadedModel loadedModel = AssimpHelper::LoadModel(params.path);
		if (loadedModel.meshes.size() <= 0) {
			triple::log::Logger::ModuleWarn(
			    "AssetService",
			    "(model: {}) the model has no meshes and as a result was not loaded", params.path);
			return std::nullopt;
		}

		Model engineModel;
		engineModel.meshes.reserve(loadedModel.meshes.size());
		processMeshes(engineModel, loadedModel, m_manager);

		return std::move(engineModel);
	}
} // namespace triple::game