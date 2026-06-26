#include "triple/game/ecs/BoundsSystem.h"

#include "triple/game/asset/AssetService.h"

#include "triple/game/utils/BoundsUtils.h"

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/MeshComponent.h"
#include "triple/game/ecs/BoundsComponent.h"

namespace triple::game {
	void BoundsSystem::onUpdate(entt::registry &registry, AssetService *assets) {
		auto view = registry.view<TransformComponent, MeshComponent, BoundsComponent>();
		for (auto [entity, transform, mesh, bounds] : view.each()) {
			if (mesh.modelIndex == core::INVALID_INDEX || assets == nullptr)
				continue;

			const Model *model = assets->getModel(mesh.modelIndex);
			if (!model)
				continue;

			BoundsUtils::transformAABB(model->boundsMin, model->boundsMax, transform.worldMatrix,
			                           bounds.worldMin, bounds.worldMax);
		}
	}
} // namespace triple::game