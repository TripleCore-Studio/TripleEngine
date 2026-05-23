#ifndef SCENE_TYPES_H
#define SCENE_TYPES_H

#include "triple/core/base/CoreTypes.h"

namespace triple::core {
	using Entity = Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();

	using ComponentTypeID = Index;
	inline constexpr ComponentTypeID INVALID_COMPONENT_TYPE_ID =
	    (std::numeric_limits<ComponentTypeID>::max)();
} // namespace triple::core

#endif // SCENE_TYPES_H
