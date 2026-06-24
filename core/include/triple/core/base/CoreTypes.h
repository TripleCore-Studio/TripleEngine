#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <cstdint>
#include <limits>

namespace triple::core {
	using Index = uint32_t;
	inline constexpr Index INVALID_INDEX = (std::numeric_limits<Index>::max)();

	using Handle = uint32_t;
	inline constexpr Handle INVALID_HANDLE = (std::numeric_limits<Handle>::max)();

	using EntityId = uint32_t;
	inline constexpr EntityId INVALID_ENTITY_ID = (std::numeric_limits<EntityId>::max)();
} // namespace triple::core

#endif // CORE_TYPES_H
