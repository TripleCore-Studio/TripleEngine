#ifndef QUAT_H
#define QUAT_H

#include "triple/math/Vec3.h"

namespace triple::math {
    struct Quat {
        float w, x, y, z;
        Quat() : w(1), x(0), y(0), z(0) {}
        Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
        static Quat fromEulerAngles(const Vec3& eulerDeg);
        Quat operator*(const Quat& rhs) const;
        Quat normalized() const;
        Vec3 toEulerAngles() const;
    };
}

#endif // QUAT_H
