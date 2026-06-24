#ifndef MAT4_OPERATIONS_H
#define MAT4_OPERATIONS_H

#include "triple/math/Mat4.h"
#include "triple/math/Vec3.h"

namespace triple::math {
	class Mat4Operations {
	public:
		static Mat4 translate(const Vec3 &v);
		static Mat4 scale(const Vec3 &v);
		static Mat4 rotate(float radians, const Vec3 &axis);

		static Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up);
		static Mat4 perspective(float fov, float aspect, float nearP, float farP);
		static Mat4 ortho(float left, float right, float bottom, float top, float nearP,
		                  float farP);

		static Mat4 multiply(const Mat4 &a, const Mat4 &b);
		static Mat4 inverse(const Mat4 &m);
	};
} // namespace triple::math

#endif // MAT4_OPERATIONS_H
