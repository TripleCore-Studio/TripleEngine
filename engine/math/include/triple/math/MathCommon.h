#ifndef TRIPLE_MATH_COMMON_H
#define TRIPLE_MATH_COMMON_H

#include "triple/math/Vec3.h"

namespace triple::math {
	constexpr float kDeg2Rad = 3.14159265f / 180.0f;

	template <typename T>
	[[nodiscard]] constexpr T clamp(const T &v, const T &min, const T &max) {
		return (v < min) ? min : (v > max) ? max : v;
	}

	[[nodiscard]] Vec3 normalize(const Vec3 &v);

	[[nodiscard]] float dot(const Vec3 &a, const Vec3 &b);

	[[nodiscard]] Vec3 cross(const Vec3 &a, const Vec3 &b);

	[[nodiscard]] inline float radians(float degree) { return degree * kDeg2Rad; }

	[[nodiscard]] float length(const Vec3 &v);

	[[nodiscard]] float lengthSquared(const Vec3 &v);
} // namespace triple::math

#endif // TRIPLE_MATH_COMMON_H
