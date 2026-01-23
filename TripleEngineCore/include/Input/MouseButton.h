#ifndef MOUSE_BUTTON_CODE_H
#define MOUSE_BUTTON_CODE_H

#include "ExportMacros.h"

namespace TripleEngineCore::Input {
	enum class CORE_API MouseButton {
		Unknown = -1,

		Left,
		Right,
		Middle,
		Button4,
		Button5,
	};
}

#endif // MOUSE_BUTTON_CODE_H
