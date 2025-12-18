#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <cstdint>
#include <limits>

namespace TripleEngineCore {
	using Index = uint32_t;
	inline constexpr Index INVALID_INDEX = std::numeric_limits<Index>::max();
} // namespace TripleEngineCore

#endif // CORE_TYPES_H
