#include "triple/game/ecs/BoundsSystem.h"

#include "triple/game/asset/AssetManager.h"

#include "triple/game/utils/BoundsUtils.h"

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/MeshRendererComponent.h"
#include "triple/game/ecs/BoundsComponent.h"

namespace triple::game {
	void BoundsSystem::onUpdate(entt::registry &registry, AssetManager *assets) {
		auto view = registry.view<TransformComponent, MeshRendererComponent, BoundsComponent>();
		for (auto [entity, transform, mesh, bounds] : view.each()) {
			if (!mesh.model.isValid() || assets == nullptr)
				continue;

			const Model *model = assets->storageFor<Model>().get(mesh.model);
			if (!model)
				continue;

			BoundsUtils::transformAABB(model->boundsMin, model->boundsMax, transform.worldMatrix,
			                           bounds.worldMin, bounds.worldMax);
		}
	}
} // namespace triple::game