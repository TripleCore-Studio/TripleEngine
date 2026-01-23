#ifndef KEY_ACTION_h
#define KEY_ACTION_h

#include <ExportMacros.h>

namespace TripleEngineCore::Event {
	enum class CORE_API KeyAction {
		Press,
		Release,
		Repeat
	};
}

#endif // KEY_ACTION_h
