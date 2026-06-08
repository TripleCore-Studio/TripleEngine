#ifndef CHILDREN_COMPONENT_H
#define CHILDREN_COMPONENT_H

#include <vector>
#include "Scene.h"

namespace triple::game {
	struct ChildrenComponent {
		std::vector<Entity> children;
	};
} // namespace triple::game

#endif // CHILDREN_COMPONENT_H
