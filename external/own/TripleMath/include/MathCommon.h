#ifndef TRIPLE_MATH_COMMON_H
#define TRIPLE_MATH_COMMON_H

#include "TripleMathExports.h"
#include "Vec3.h"

namespace TripleEngineCore::TripleMath {

    template<typename T>
    constexpr T clamp(const T& v, const T& min, const T& max)
    {
        return (v < min) ? min : (v > max) ? max : v;
    }

    TRIPLEMATH_API Vec3 normalize(const Vec3& v);

    TRIPLEMATH_API float dot(const Vec3& a, const Vec3& b);

    TRIPLEMATH_API Vec3 cross(const Vec3& a, const Vec3& b);
}

#endif // TRIPLE_MATH_COMMON_H
