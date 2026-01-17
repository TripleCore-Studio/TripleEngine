#ifndef PARENT_COMPONENT_H
#define PARENT_COMPONENT_H

#include "Scene/Scene.h"

namespace TripleEngineCore::Scene {
	struct ParentComponent {
		Entity parent = 0; // 0 = no parent (root)
	};
}

#endif // PARENT_COMPONENT_H
