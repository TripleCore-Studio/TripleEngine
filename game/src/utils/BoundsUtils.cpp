#include "triple/game/utils/BoundsUtils.h"

#include <cmath>

using namespace triple::math;

namespace triple::game {
	void BoundsUtils::transformAABB(const Vec3 &localMin, const Vec3 &localMax,
	                                const Mat4 &worldMatrix, Vec3 &outWorldMin, Vec3 &outWorldMax) {
		Vec3 center = (localMin + localMax) * 0.5f;
		Vec3 extents = (localMax - localMin) * 0.5f;

		Vec3 newCenter = Vec3(worldMatrix * Vec4(center, 1.0f));

		Vec3 newExtents;
		newExtents.x = std::abs(worldMatrix.data[0]) * extents.x +
		               std::abs(worldMatrix.data[4]) * extents.y +
		               std::abs(worldMatrix.data[8]) * extents.z;
		newExtents.y = std::abs(worldMatrix.data[1]) * extents.x +
		               std::abs(worldMatrix.data[5]) * extents.y +
		               std::abs(worldMatrix.data[9]) * extents.z;
		newExtents.z = std::abs(worldMatrix.data[2]) * extents.x +
		               std::abs(worldMatrix.data[6]) * extents.y +
		               std::abs(worldMatrix.data[10]) * extents.z;

		outWorldMin = newCenter - newExtents;
		outWorldMax = newCenter + newExtents;
	}
} // namespace triple::game