#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <cstdint>
#include <limits>

namespace triple::gfx {
	using GPUHandle = uint32_t;
	inline constexpr GPUHandle kInvalidGpuHandle = (std::numeric_limits<uint32_t>::max)();
} // namespace triple::gfx

#endif