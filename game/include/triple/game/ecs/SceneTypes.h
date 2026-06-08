#ifndef SCENE_TYPES_H
#define SCENE_TYPES_H

#include "triple/core/base/CoreTypes.h"

namespace triple::game {
	using Entity = core::Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();

	using ComponentTypeID = core::Index;
	inline constexpr ComponentTypeID INVALID_COMPONENT_TYPE_ID =
	    (std::numeric_limits<ComponentTypeID>::max)();
} // namespace triple::game

#endif // SCENE_TYPES_H
