#ifndef MOUSE_BUTTON_EVENT_H
#define MOUSE_BUTTON_EVENT_H

#include "Event/Event.h"
#include "Event/KeyAction.h"
#include "Input/MouseButton.h"

namespace TripleEngineCore::Event {
	class CORE_API MouseButtonEvent : public Event {
	public:
		MouseButtonEvent(Input::MouseButton button, KeyAction action, int mods)
			: button(button), action(action), mods(mods) {
		}

		EventID getTypeID() const override {
			return typeid(MouseButtonEvent);
		}

		Input::MouseButton getButton() const { return button; }
		KeyAction getAction() const { return action; }
		int getMods() const { return mods; }

		bool isPressed() const { return action == KeyAction::Press; }
		bool isReleased() const { return action == KeyAction::Release; }
		bool isRepeat() const { return action == KeyAction::Repeat; }

	private:
		Input::MouseButton button;
		KeyAction action;
		int mods;
	};
}

#endif // MOUSE_BUTTON_EVENT_H
