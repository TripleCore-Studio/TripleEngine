#ifndef PARENT_COMPONENT_H
#define PARENT_COMPONENT_H

#include "Scene.h"

namespace triple::game {
	struct ParentComponent {
		Entity parent = 0; // 0 = no parent (root)
	};
} // namespace triple::game

#endif // PARENT_COMPONENT_H
