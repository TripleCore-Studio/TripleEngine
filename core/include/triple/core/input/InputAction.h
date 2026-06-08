#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include <triple/core/input/KeyCode.h>
#include <triple/core/input/MouseButton.h>

namespace triple::core {
	enum class InputTriggerType { Key, MouseButton };

	enum class TriggerState { Pressed, Held, Released };

	struct InputTrigger {
		InputTriggerType type;
		TriggerState state;

		union {
			core::KeyCode key;
			core::MouseButton mouse;
		};
	};
} // namespace triple::core

#endif // INPUT_ACTION_H
