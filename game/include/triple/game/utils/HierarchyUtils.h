#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class HierarchyUtils {
	public:
		static void setParent(entt::registry &registry, entt::entity child, entt::entity parent);
		static void destroyEntityRecursive(entt::registry &registry, entt::entity entity);
	};
} // namespace triple::game