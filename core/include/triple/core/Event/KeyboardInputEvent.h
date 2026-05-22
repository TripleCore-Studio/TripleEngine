#ifndef KEYBOARD_INPUT_EVENT_H
#define KEYBOARD_INPUT_EVENT_H

#include <triple/core/ExportMacros.h>

#include "Event.h"
#include "KeyAction.h"
#include "triple/core/Input/KeyCode.h"

namespace triple::core {
	class CORE_API KeyboardInputEvent : public Event {
	public:
		KeyboardInputEvent(KeyCode key, KeyAction action, int mods)
			: m_key(key), m_action(action), m_mods(mods) {
		}

		EventID getTypeID() const override {
			return typeid(KeyboardInputEvent);
		}

		KeyCode getKey() const { return m_key; }
		KeyAction getAction() const { return m_action; }
		int getMods() const { return m_mods; }

		bool isPressed() const { return m_action == KeyAction::Press; }
		bool isReleased() const { return m_action == KeyAction::Release; }
		bool isRepeat() const { return m_action == KeyAction::Repeat; }
	private:
		KeyCode m_key;
		KeyAction m_action;
		int m_mods;
	};
}

#endif // KEYBOARD_INPUT_EVENT_H
