#include "triple/game/ecs/EntityFactory.h"

#include <triple/log/Logger.h>

#include "triple/game/asset/AssetManager.h"
#include "triple/game/asset/MaterialInstance.h"

#include "triple/game/ecs/MeshRendererComponent.h"

namespace triple::game {
	std::vector<std::vector<MaterialInstance>> makeMaterialInstancesForModel(const Model &model);

	entt::entity EntityFactory::createMeshEntity(TypedAssetID<Model> model,
	                                             TransformComponent transform) {
		entt::entity entity = m_registry.create();
		m_registry.emplace<TransformComponent>(entity, transform);

		const Model *modelPtr = m_assetManager.storageFor<Model>().get(model);
		if (!modelPtr) {
			triple::log::Logger::moduleError("EntityFactory",
			                                 "createMeshEntity: invalid or missing Model asset, "
			                                 "entity created without MeshRendererComponent");
			return entity;
		}

		MeshRendererComponent meshRenderer;
		meshRenderer.model = model;
		meshRenderer.materialInstance = makeMaterialInstancesForModel(*modelPtr);

		m_registry.emplace<MeshRendererComponent>(entity, std::move(meshRenderer));

		return entity;
	}

	std::vector<std::vector<MaterialInstance>> makeMaterialInstancesForModel(const Model &model) {
		std::vector<std::vector<MaterialInstance>> result;
		result.reserve(model.meshes.size());

		for (const Mesh &mesh : model.meshes) {
			std::vector<MaterialInstance> meshInstances;
			meshInstances.reserve(mesh.primitives.size());

			for (const Primitive &primitive : mesh.primitives) {
				meshInstances.emplace_back(primitive.material);
			}

			result.push_back(std::move(meshInstances));
		}

		return result;
	}
} // namespace triple::game