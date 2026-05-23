#include "triple/math/MathCommon.h"
#include <cmath>

namespace triple::math {
	Vec3 normalize(const Vec3 &v) {
		float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len == 0.0f)
			return Vec3{0.0f, 0.0f, 0.0f};
		return Vec3{v.x / len, v.y / len, v.z / len};
	}
	float dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	Vec3 cross(const Vec3 &a, const Vec3 &b) {
		return Vec3{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
	}
} // namespace triple::math