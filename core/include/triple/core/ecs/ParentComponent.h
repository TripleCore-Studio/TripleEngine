#ifndef PARENT_COMPONENT_H
#define PARENT_COMPONENT_H

#include "Scene.h"

namespace triple::core {
	struct ParentComponent {
		Entity parent = 0; // 0 = no parent (root)
	};
} // namespace triple::core

#endif // PARENT_COMPONENT_H
