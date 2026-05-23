#ifndef CHILDREN_COMPONENT_H
#define CHILDREN_COMPONENT_H

#include <vector>
#include "Scene.h"

namespace triple::core {
	struct ChildrenComponent {
		std::vector<Entity> children;
	};
} // namespace triple::core

#endif // CHILDREN_COMPONENT_H
