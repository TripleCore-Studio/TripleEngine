#ifndef MOUSE_BUTTON_EVENT_H
#define MOUSE_BUTTON_EVENT_H

#include <triple/core/base/ExportMacros.h>

#include "EventBase.h"
#include "KeyAction.h"
#include "triple/core/input/MouseButton.h"

namespace triple::core {
	class CORE_API MouseButtonEvent : public EventBase<MouseButtonEvent> {
	public:
		MouseButtonEvent(MouseButton button, KeyAction action, int mods)
		    : m_button(button), m_action(action), m_mods(mods) {}

		[[nodiscard]] MouseButton getButton() const { return m_button; }
		[[nodiscard]] KeyAction getAction() const { return m_action; }
		[[nodiscard]] int getMods() const { return m_mods; }

		[[nodiscard]] bool isPressed() const { return m_action == KeyAction::Press; }
		[[nodiscard]] bool isReleased() const { return m_action == KeyAction::Release; }
		[[nodiscard]] bool isRepeat() const { return m_action == KeyAction::Repeat; }

	private:
		MouseButton m_button;
		KeyAction m_action;
		int m_mods;
	};
} // namespace triple::core

#endif // MOUSE_BUTTON_EVENT_H
