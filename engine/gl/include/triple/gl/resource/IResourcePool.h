#pragma once

#include <triple/gfx/GfxTypes.h>

namespace triple::gl {
	class IResourcePool {
	public:
		virtual ~IResourcePool() = default;
		virtual bool destroy(gfx::GpuHandle handle) = 0;
	};
} // namespace triple::gl