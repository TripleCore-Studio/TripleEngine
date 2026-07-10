#include "triple/game/asset/ModelLoader.h"

#include <assert.h>

#include <triple/log/Logger.h>

#include <triple/gfx/GfxDescs.h>

#include "triple/game/asset/AssimpHelper.h"
#include "triple/game/asset/AssetManager.h"
#include "triple/game/asset/DefaultAssets.h"

namespace triple::game {
	static uint32_t sizeOfVertexAttrib(gfx::VertexAttribType type) {
		switch (type) {
			case gfx::VertexAttribType::Float:
				return 4;
			case gfx::VertexAttribType::Vec2:
				return 8;
			case gfx::VertexAttribType::Vec3:
				return 12;
			case gfx::VertexAttribType::Vec4:
				return 16;
		};

		assert(false && "Unknown VertexAttribType");
		return 0;
	}

	gfx::VertexLayout makeStandardVertexLayout() {
		gfx::VertexLayout layout;
		uint32_t offset = 0;

		auto addAttr = [&](std::string semantic, gfx::VertexAttribType type, uint32_t location) {
			gfx::VertexAttributeDesc attr;
			attr.semantic = std::move(semantic);
			attr.type = type;
			attr.location = location;
			attr.offset = offset;
			layout.attributes.push_back(attr);
			offset += sizeOfVertexAttrib(type);
		};

		addAttr("POSITION", gfx::VertexAttribType::Vec3, 0);
		addAttr("NORMAL", gfx::VertexAttribType::Vec3, 1);
		addAttr("TANGENT", gfx::VertexAttribType::Vec3, 2);
		addAttr("BITANGENT", gfx::VertexAttribType::Vec3, 3);
		addAttr("TEXCOORD0", gfx::VertexAttribType::Vec2, 4);

		layout.stride = offset;
		return layout;
	}

	void appendVertexRaw(std::vector<uint8_t> &out, const gfx::Vertex &v) {
		auto push = [&out](const void *data, size_t size) {
			const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data);
			out.insert(out.end(), bytes, bytes + size);
		};

		push(&v.position, sizeof(v.position));
		push(&v.normal, sizeof(v.normal));
		push(&v.tangent, sizeof(v.tangent));
		push(&v.bitangent, sizeof(v.bitangent));
		push(&v.uv, sizeof(v.uv));
	}
} // namespace triple::game

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
		AssimpHelper::LoadedMaterial loadedMaterial = loadedModel.materials[mesh.materialIndex];
		AssimpHelper::LoadedTexture difftex;
		bool isEmpty = true;
		if (!loadedMaterial.diffuseTextures.empty()) {
			difftex = loadedMaterial.diffuseTextures[0];
			isEmpty = false;
		}

		Material material;
		material.shader = getDefaultOrAssert<Shader>(manager, kDefaultShaderName);
		material.blendMode = loadedMaterial.blendMode;

		// TODO: model loading is currently superficial — material params (albedoColor,
		// metallic, roughness) and metallic/normal/roughness textures are not read from
		// the Assimp material at all; always falls back to hardcoded defaults.
		material.params[kAlbedoColorParam.data()] =
		    MaterialParamValue{{1.0f, 1.0f, 1.0f, loadedMaterial.opacity}};
		material.params[kMetallicParam.data()] = MaterialParamValue{{0.0f}};
		material.params[kRoughnessParam.data()] = MaterialParamValue{{1.0f}};

		if (!isEmpty) {
			Texture texture;
			texture.width = static_cast<uint16_t>(difftex.width);
			texture.height = static_cast<uint16_t>(difftex.height);
			texture.channels = static_cast<uint8_t>(difftex.channels);
			texture.pixels = std::move(difftex.pixels);

			material.textures[kAlbedoMapSlot.data()] =
			    manager->create<Texture>(loadedMaterial.name + difftex.name, std::move(texture));
		} else {
			material.textures[kAlbedoMapSlot.data()] =
			    getDefaultOrAssert<Texture>(manager, kDefaultAlbedoTextureName);
		}

		material.textures[kMetallicMapSlot.data()] =
		    getDefaultOrAssert<Texture>(manager, kDefaultMetallicTextureName);
		material.textures[kNormalMapSlot.data()] =
		    getDefaultOrAssert<Texture>(manager, kDefaultNormalTextureName);
		material.textures[kRoughnessMapSlot.data()] =
		    getDefaultOrAssert<Texture>(manager, kDefaultRoughnessTextureName);

		return manager->create<Material>(loadedMaterial.name, material);
	}

	void processMeshes(Model &engineModel, const AssimpHelper::LoadedModel &loadedModel,
	                   AssetManager *manager) {
		engineModel.vertexLayout = makeStandardVertexLayout();

		for (const auto &loadedMesh : loadedModel.meshes) {
			Mesh eMesh;
			eMesh.name = loadedMesh.name;

			uint32_t baseVertex = static_cast<uint32_t>(engineModel.vertices.size());
			uint32_t baseIndex = static_cast<uint32_t>(engineModel.indices.size());

			for (auto &vert : loadedMesh.vertices) {
				appendVertexRaw(engineModel.vertices, vert);

				engineModel.boundsMin.x = std::min(engineModel.boundsMin.x, vert.position.x);
				engineModel.boundsMin.y = std::min(engineModel.boundsMin.y, vert.position.y);
				engineModel.boundsMin.z = std::min(engineModel.boundsMin.z, vert.position.z);

				engineModel.boundsMax.x = std::max(engineModel.boundsMax.x, vert.position.x);
				engineModel.boundsMax.y = std::max(engineModel.boundsMax.y, vert.position.y);
				engineModel.boundsMax.z = std::max(engineModel.boundsMax.z, vert.position.z);
			}

			for (uint32_t idx : loadedMesh.indices) {
				engineModel.indices.push_back(idx + baseVertex);
			}

			Primitive prim;
			prim.indexOffset = baseIndex;
			prim.indexCount = static_cast<uint32_t>(loadedMesh.indices.size());
			prim.material = processMaterial(loadedMesh, loadedModel, manager);
			eMesh.primitives.push_back(prim);

			engineModel.meshes.push_back(std::move(eMesh));
		}
	}

	std::optional<Model> ModelLoader::load(const ModelLoadParams &params) {
		AssimpHelper::LoadedModel loadedModel = AssimpHelper::loadModel(params.path);
		if (loadedModel.meshes.size() <= 0) {
			triple::log::Logger::ModuleWarn(
			    "ModelLoader", "(model: {}) the model has no meshes and as a result was not loaded",
			    params.path);
			return std::nullopt;
		}

		Model engineModel;
		engineModel.meshes.reserve(loadedModel.meshes.size());
		processMeshes(engineModel, loadedModel, m_manager);

		return std::move(engineModel);
	}
} // namespace triple::game