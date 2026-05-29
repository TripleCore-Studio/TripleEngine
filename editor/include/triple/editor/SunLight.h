#pragma once

#include <triple/math/Vec3.h>

namespace triple::editor {
	struct SunLight {
		math::Vec3 color;
		float elevation = 45.0f;
		float azimuth = 0.0f;
		float intensity = 10.0f;
	};
} // namespace triple::editor