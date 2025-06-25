#ifndef TRIPLE_ENGINE_CORE_H
#define TRIPLE_ENGINE_CORE_H

#ifdef _WIN32
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif

#include <string>

namespace TripleEngineCore {
	class CORE_API TripleEngineCore {
	public:
		TripleEngineCore();
	};
}

#endif // TRIPLE_ENGINE_CORE_H