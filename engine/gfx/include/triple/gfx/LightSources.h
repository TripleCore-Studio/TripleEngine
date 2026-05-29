#pragma once

#include <triple/math/Vec3.h>

namespace triple::gfx {
	struct SunLight {
		math::Vec3 direction;
		math::Vec3 color;
		float intensity;
	};

	struct CameraLight {
		math::Vec3 color;
		float intensity;
		float radius;
	};
} // namespace triple::gfx