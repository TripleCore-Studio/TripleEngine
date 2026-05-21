#ifndef SCENE_TYPES_H
#define SCENE_TYPES_H

#include "Core/CoreTypes.h"

namespace TripleEngineCore::Scene {
	using Entity = Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();

	using ComponentTypeID = Index;
	inline constexpr ComponentTypeID INVALID_COMPONENT_TYPE_ID = (std::numeric_limits<ComponentTypeID>::max)();
}

#endif // SCENE_TYPES_H
