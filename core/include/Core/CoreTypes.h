#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <cstdint>
#include <limits>

namespace TripleEngineCore {
	using Index = uint32_t;
	inline constexpr Index INVALID_INDEX = (std::numeric_limits<Index>::max)();

	using Handle = uint32_t; 
	inline constexpr Handle INVALID_HANDLE = (std::numeric_limits<Handle>::max)();

	using GPUHandle = TripleEngineCore::Handle;
	inline constexpr GPUHandle INVALID_GPU_HANDLE = TripleEngineCore::INVALID_HANDLE;
} // namespace TripleEngineCore

#endif // CORE_TYPES_H
