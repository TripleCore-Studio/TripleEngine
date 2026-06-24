#include "triple/game/utils/HierarchyUtils.h"

#include "triple/game/ecs/ParentComponent.h"
#include "triple/game/ecs/ChildrenComponent.h"

namespace triple::game {
	void HierarchyUtils::setParent(entt::registry &registry, entt::entity child,
	                               entt::entity parent) {
		if (auto *oldParentComp = registry.try_get<ParentComponent>(child)) {
			auto &oldChildren = registry.get<ChildrenComponent>(oldParentComp->parent).children;
			std::erase(oldChildren, child);
		}

		if (parent == entt::null) {
			registry.remove<ParentComponent>(child);
			return;
		}

		registry.emplace_or_replace<ParentComponent>(child, parent);

		if (!registry.all_of<ChildrenComponent>(parent)) {
			registry.emplace<ChildrenComponent>(parent);
		}
		registry.get<ChildrenComponent>(parent).children.push_back(child);
	}
	void HierarchyUtils::destroyEntityRecursive(entt::registry &registry, entt::entity entity) {
		if (auto *children = registry.try_get<ChildrenComponent>(entity)) {
			auto childrenCopy = children->children;
			for (auto child : childrenCopy) {
				destroyEntityRecursive(registry, child);
			}
		}
		setParent(registry, entity, entt::null);
		registry.destroy(entity);
	}
} // namespace triple::game