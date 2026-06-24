#include "triple/game/ecs/TransformSystem.h"

#include "triple/game/utils/TransformUtils.h"

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/ParentComponent.h"
#include "triple/game/ecs/ChildrenComponent.h"

namespace triple::game {
	void recursiveUpdateTransform(entt::registry &reg, entt::entity entity, TransformComponent &t,
	                              const math::Mat4 &parentWorldMatrix = math::Mat4::identity()) {
		t.worldMatrix = parentWorldMatrix * TransformUtils::getModelMatrix(t);

		if (auto *childComp = reg.try_get<ChildrenComponent>(entity)) {
			for (auto &e : childComp->children) {
				auto &tr = reg.get<TransformComponent>(e);
				recursiveUpdateTransform(reg, e, tr, t.worldMatrix);
			}
		}
	}

	void TransformSystem::onUpdate(entt::registry &reg, float dt) {
		auto view = reg.view<TransformComponent>(entt::exclude<ParentComponent>);
		for (auto [entity, t] : view.each()) {
			recursiveUpdateTransform(reg, entity, t);
		}
	}
} // namespace triple::game