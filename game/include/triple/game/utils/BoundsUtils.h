#pragma once

#include <triple/math/Vec3.h>
#include <triple/math/Mat4.h>

namespace triple::game {
	class BoundsUtils {
	public:
		static void transformAABB(const math::Vec3 &localMin, const math::Vec3 &localMax,
		                          const math::Mat4 &worldMatrix, math::Vec3 &outWorldMin,
		                          math::Vec3 &outWorldMax);
	};
} // namespace triple::game