#ifndef EXPORT_MACROS_H
#define EXPORT_MACROS_H

#if defined(_WIN32) || defined(_WIN64)
#define TRIPLEENGINE_WINDOWS
#endif

// ===================== CORE ======================
#if defined(TRIPLEENGINE_WINDOWS)
#if defined(CORE_EXPORTS)
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif

// ================= TESTS ========================
#if defined(TRIPLEENGINE_WINDOWS)
#if defined(CORE_EXPORTS_FOR_TESTS)
#define CORE_API_FOR_TESTS __declspec(dllexport)
#else
#define CORE_API_FOR_TESTS
#endif
#else
#define CORE_API_FOR_TESTS
#endif

#endif // EXPORT_MACROS_H