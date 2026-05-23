#ifndef ENTITY_H
#define ENTITY_H

#include "triple/core/base/CoreTypes.h"

namespace triple::core {
	using Entity = Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();
} // namespace triple::core

#endif // ENTITY_H
