#ifndef MAT4_OPERATIONS_H
#define MAT4_OPERATIONS_H

#include "Mat4.h"
#include "Vec3.h"
#include "TripleMathExports.h"


namespace TripleEngineCore::TripleMath {

    TRIPLEMATH_API Mat4 translate(const Vec3& v);
    TRIPLEMATH_API Mat4 scale(const Vec3& v);
    TRIPLEMATH_API Mat4 rotate(float radians, const Vec3& axis);

    TRIPLEMATH_API Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    TRIPLEMATH_API Mat4 perspective(float fov, float aspect, float nearP, float farP);
    TRIPLEMATH_API Mat4 ortho(float left, float right, float bottom, float top, float nearP, float farP);

    TRIPLEMATH_API Mat4 multiply(const Mat4& a, const Mat4& b);
}

#endif // MAT4_OPERATIONS_H
