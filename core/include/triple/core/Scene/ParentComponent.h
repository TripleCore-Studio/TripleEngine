#ifndef PARENT_COMPONENT_H
#define PARENT_COMPONENT_H

#include "triple/core/Scene/Scene.h"

namespace triple::core {
	struct ParentComponent {
		Entity parent = 0; // 0 = no parent (root)
	};
}

#endif // PARENT_COMPONENT_H
