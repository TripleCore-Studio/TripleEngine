#ifndef MOUSE_MOVE_EVENT_H
#define MOUSE_MOVE_EVENT_H

#include "Event/Event.h"

namespace TripleEngineCore::Event {
	class CORE_API MouseMoveEvent : public Event {
	public:
		MouseMoveEvent(float x, float y)
			: m_x(x), m_y(y) {
		}

		EventID getTypeID() const override {
			return typeid(MouseMoveEvent);
		}

		float getX() const { return m_x; }
		float getY() const { return m_y; }
	private:
		float m_x;
		float m_y;
	};
}

#endif // MOUSE_MOVE_EVENT_H
