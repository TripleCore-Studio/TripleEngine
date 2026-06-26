#pragma once

#include <triple/math/Vec3.h>

namespace triple::game {
	struct BoundsComponent {
		math::Vec3 worldMin{0, 0, 0};
		math::Vec3 worldMax{0, 0, 0};
	};
} // namespace triple::game