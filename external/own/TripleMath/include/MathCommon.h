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
}

#endif // TRIPLE_MATH_COMMON_H
