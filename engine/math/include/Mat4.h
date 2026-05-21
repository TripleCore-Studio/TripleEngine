#ifndef MAT4_H
#define MAT4_H

#include "TripleMathExports.h"
#include "Vec4.h"

namespace TripleEngineCore::TripleMath {

    struct TRIPLEMATH_API Mat4
    {
        float data[16];

        Mat4();
        explicit Mat4(float diagonal);

        Mat4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);

        static Mat4 identity();

        Vec4 operator*(const Vec4& v) const;
        Mat4 operator*(const Mat4& other) const;
    };
}

#endif // MAT4_H
