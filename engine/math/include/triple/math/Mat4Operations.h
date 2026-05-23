#ifndef MAT4_OPERATIONS_H
#define MAT4_OPERATIONS_H

#include "triple/math/Mat4.h"
#include "triple/math/Vec3.h"

namespace triple::math {

	Mat4 translate(const Vec3 &v);
	Mat4 scale(const Vec3 &v);
	Mat4 rotate(float radians, const Vec3 &axis);

	Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up);
	Mat4 perspective(float fov, float aspect, float nearP, float farP);
	Mat4 ortho(float left, float right, float bottom, float top, float nearP, float farP);

	Mat4 multiply(const Mat4 &a, const Mat4 &b);
} // namespace triple::math

#endif // MAT4_OPERATIONS_H
