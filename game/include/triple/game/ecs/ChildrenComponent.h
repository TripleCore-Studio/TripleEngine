#ifndef CHILDREN_COMPONENT_H
#define CHILDREN_COMPONENT_H

#include <vector>
#include <entt/entity/entity.hpp>

namespace triple::game {
	struct ChildrenComponent {
		std::vector<entt::entity> children;
	};
} // namespace triple::game

#endif // CHILDREN_COMPONENT_H
