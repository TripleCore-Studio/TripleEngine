#ifndef MOUSE_MOVE_EVENT_H
#define MOUSE_MOVE_EVENT_H

#include "EventBase.h"

namespace triple::core {
	class CORE_API MouseMoveEvent : public EventBase<MouseMoveEvent> {
	public:
		MouseMoveEvent(float x, float y) : m_x(x), m_y(y) {}

		[[nodiscard]] float getX() const { return m_x; }
		[[nodiscard]] float getY() const { return m_y; }

	private:
		float m_x;
		float m_y;
	};
} // namespace triple::core

#endif // MOUSE_MOVE_EVENT_H
