#ifndef KEYBOARD_INPUT_EVENT_H
#define KEYBOARD_INPUT_EVENT_H

#include <triple/core/base/ExportMacros.h>

#include "EventBase.h"
#include "KeyAction.h"
#include "triple/core/input/KeyCode.h"

namespace triple::core {
	class CORE_API KeyboardInputEvent : public EventBase<KeyboardInputEvent> {
	public:
		KeyboardInputEvent(KeyCode key, KeyAction action, int mods)
		    : m_key(key), m_action(action), m_mods(mods) {}

		[[nodiscard]] KeyCode getKey() const { return m_key; }
		[[nodiscard]] KeyAction getAction() const { return m_action; }
		[[nodiscard]] int getMods() const { return m_mods; }

		[[nodiscard]] bool isPressed() const { return m_action == KeyAction::Press; }
		[[nodiscard]] bool isReleased() const { return m_action == KeyAction::Release; }
		[[nodiscard]] bool isRepeat() const { return m_action == KeyAction::Repeat; }

	private:
		KeyCode m_key;
		KeyAction m_action;
		int m_mods;
	};
} // namespace triple::core

#endif // KEYBOARD_INPUT_EVENT_H