#ifndef ENTITY_H
#define ENTITY_H

#include "triple/core/CoreTypes.h"

namespace triple::core {
	using Entity = Index;
	inline constexpr Entity INVALID_ENTITY = (std::numeric_limits<Entity>::max)();
}

#endif // ENTITY_H
