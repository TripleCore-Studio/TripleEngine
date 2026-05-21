#ifndef VEC3_H
#define VEC3_H

#include "TripleMathExports.h"
#include "Vec2.h"

namespace TripleEngineCore::TripleMath {

    struct TRIPLEMATH_API Vec3 : public Vec2 {
        float z;

        Vec3() : Vec2(), z(0) {}
        Vec3(float xx, float yy, float zz) : Vec2(xx, yy), z(zz) {}
        Vec3(const Vec2& v, float zz) : Vec2(v), z(zz) {}

        Vec3 operator+(const Vec3& other) const;
        Vec3 operator-(const Vec3& other) const;
        Vec3 operator*(float scalar) const;
        Vec3 operator/(float scalar) const;

        Vec3& operator+=(const Vec3& other);
        Vec3& operator-=(const Vec3& other);
        Vec3& operator*=(float scalar);
        Vec3& operator/=(float scalar);

        float length() const;
        Vec3 normalized() const;
        const float* data() const { return &x; }
    };

}

#endif // VEC3_H
