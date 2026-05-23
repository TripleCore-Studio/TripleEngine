#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <cstdint>
#include <limits>

namespace triple::core {
	using Index = uint32_t;
	inline constexpr Index INVALID_INDEX = (std::numeric_limits<Index>::max)();

	using Handle = uint32_t; 
	inline constexpr Handle INVALID_HANDLE = (std::numeric_limits<Handle>::max)();
} // namespace TripleEngineCore

#endif // CORE_TYPES_H
