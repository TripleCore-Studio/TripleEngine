#ifndef KEYBOARD_INPUT_EVENT_H
#define KEYBOARD_INPUT_EVENT_H

#include "Event/Event.h"
#include "Event/KeyAction.h"
#include "Input/KeyCode.h"

namespace TripleEngineCore::Event {
	class CORE_API KeyboardInputEvent : public Event {
	public:
		KeyboardInputEvent(Input::KeyCode key, KeyAction action, int mods)
			: key(key), action(action), mods(mods) {
		}

		EventID getTypeID() const override {
			return typeid(KeyboardInputEvent);
		}

		Input::KeyCode getKey() const { return key; }
		KeyAction getAction() const { return action; }
		int getMods() const { return mods; }

		bool isPressed() const { return action == KeyAction::Press; }
		bool isReleased() const { return action == KeyAction::Release; }
		bool isRepeat() const { return action == KeyAction::Repeat; }
	private:
		Input::KeyCode key;
		KeyAction action;
		int mods;
	};
}

#endif // KEYBOARD_INPUT_EVENT_H
