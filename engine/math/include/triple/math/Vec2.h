#ifndef VEC2_H
#define VEC2_H

namespace triple::math {
	struct Vec2 {
		float x, y;

		Vec2() : x(0), y(0) {}
		Vec2(float xx, float yy) : x(xx), y(yy) {}

		Vec2 operator+(const Vec2 &other) const;
		Vec2 operator-(const Vec2 &other) const;
		Vec2 operator*(float scalar) const;
		Vec2 operator/(float scalar) const;

		Vec2 &operator+=(const Vec2 &other);
		Vec2 &operator-=(const Vec2 &other);
		Vec2 &operator*=(float scalar);
		Vec2 &operator/=(float scalar);

		float length() const;
		Vec2 normalized() const;
	};
} // namespace triple::math

#endif // VEC2_H
