#ifndef MOUSE_BUTTON_EVENT_H
#define MOUSE_BUTTON_EVENT_H

#include <triple/core/base/ExportMacros.h>

#include "Event.h"
#include "KeyAction.h"
#include "triple/core/Input/MouseButton.h"

namespace triple::core {
	class CORE_API MouseButtonEvent : public Event {
	public:
		MouseButtonEvent(MouseButton button, KeyAction action, int mods)
			: m_button(button), m_action(action), m_mods(mods) {
		}

		EventID getTypeID() const override {
			return typeid(MouseButtonEvent);
		}

		MouseButton getButton() const { return m_button; }
		KeyAction getAction() const { return m_action; }
		int getMods() const { return m_mods; }

		bool isPressed() const { return m_action == KeyAction::Press; }
		bool isReleased() const { return m_action == KeyAction::Release; }
		bool isRepeat() const { return m_action == KeyAction::Repeat; }

	private:
		MouseButton m_button;
		KeyAction m_action;
		int m_mods;
	};
}

#endif // MOUSE_BUTTON_EVENT_H
