#ifndef PARENT_COMPONENT_H
#define PARENT_COMPONENT_H

#include <entt/entity/entity.hpp>

namespace triple::game {
	struct ParentComponent {
		entt::entity parent = entt::null;
	};
} // namespace triple::game

#endif // PARENT_COMPONENT_H
