#ifndef TRIPLE_MATH_COMMON_H
#define TRIPLE_MATH_COMMON_H

namespace TripleEngineCore::TripleMath {

    template<typename T>
    constexpr T clamp(const T& v, const T& min, const T& max)
    {
        return (v < min) ? min : (v > max) ? max : v;
    }

}

#endif // TRIPLE_MATH_COMMON_H
