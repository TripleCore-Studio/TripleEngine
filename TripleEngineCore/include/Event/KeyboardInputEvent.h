#ifndef KEYBOARD_INPUT_EVENT_H
#define KEYBOARD_INPUT_EVENT_H

#include "Event/Event.h"
#include "Event/KeyAction.h"
#include "Input/KeyCode.h"

namespace TripleEngineCore::Event {
	class CORE_API KeyboardInputEvent : public Event {
	public:
		KeyboardInputEvent(Input::KeyCode key, KeyAction action, int mods)
			: m_key(key), m_action(action), m_mods(mods) {
		}

		EventID getTypeID() const override {
			return typeid(KeyboardInputEvent);
		}

		Input::KeyCode getKey() const { return m_key; }
		KeyAction getAction() const { return m_action; }
		int getMods() const { return m_mods; }

		bool isPressed() const { return m_action == KeyAction::Press; }
		bool isReleased() const { return m_action == KeyAction::Release; }
		bool isRepeat() const { return m_action == KeyAction::Repeat; }
	private:
		Input::KeyCode m_key;
		KeyAction m_action;
		int m_mods;
	};
}

#endif // KEYBOARD_INPUT_EVENT_H
