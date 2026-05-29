#ifndef TRIPLE_MATH_COMMON_H
#define TRIPLE_MATH_COMMON_H

#include "triple/math/Vec3.h"

namespace triple::math {
	constexpr float DEG2RAD = 3.14159265f / 180.0f;

	template <typename T>
	constexpr T clamp(const T &v, const T &min, const T &max) {
		return (v < min) ? min : (v > max) ? max : v;
	}

	Vec3 normalize(const Vec3 &v);

	float dot(const Vec3 &a, const Vec3 &b);

	Vec3 cross(const Vec3 &a, const Vec3 &b);

	inline float radians(float degree) { return degree * DEG2RAD; }
} // namespace triple::math

#endif // TRIPLE_MATH_COMMON_H
