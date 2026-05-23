#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <cstdint>
#include <limits>

namespace triple::gfx {
	using GPUHandle = uint32_t;
	inline constexpr GPUHandle INVALID_GPU_HANDLE = (std::numeric_limits<uint32_t>::max)();
} // namespace triple::gfx

#endif