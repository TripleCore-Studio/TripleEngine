#ifndef VEC4_H
#define VEC4_H

#include "triple/math/Vec3.h"

namespace triple::math {

	struct Vec4 : public Vec3 {
		float w;

		Vec4() : Vec3(), w(0) {}
		Vec4(float xx, float yy, float zz, float ww) : Vec3(xx, yy, zz), w(ww) {}
		Vec4(const Vec3 &v, float ww) : Vec3(v), w(ww) {}

		Vec4 operator+(const Vec4 &other) const;
		Vec4 operator-(const Vec4 &other) const;
		Vec4 operator*(float scalar) const;
		Vec4 operator/(float scalar) const;

		Vec4 &operator+=(const Vec4 &other);
		Vec4 &operator-=(const Vec4 &other);
		Vec4 &operator*=(float scalar);
		Vec4 &operator/=(float scalar);

		float length() const;
		Vec4 normalized() const;
	};

} // namespace triple::math

#endif // VEC4_H
