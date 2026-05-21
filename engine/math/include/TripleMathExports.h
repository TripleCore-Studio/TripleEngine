#ifndef TRIPLE_MATH_EXPORTS_H
#define TRIPLE_MATH_EXPORTS_H

#ifdef _WIN32
#ifdef TRIPLEMATH_EXPORTS
#define TRIPLEMATH_API __declspec(dllexport)
#else
#define TRIPLEMATH_API __declspec(dllimport)
#endif
#else
#define TRIPLEMATH_API
#endif

#endif // TRIPLE_MATH_EXPORTS_H
