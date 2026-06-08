#ifndef ENTITY_H
#define ENTITY_H

#include "triple/core/base/CoreTypes.h"

namespace triple::game {
	using Entity = Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();
} // namespace triple::game

#endif // ENTITY_H
