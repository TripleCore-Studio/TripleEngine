#ifndef MOUSE_MOVE_EVENT_H
#define MOUSE_MOVE_EVENT_H

#include "Event/Event.h"

namespace TripleEngineCore::Event {
	class CORE_API MouseMoveEvent : public Event {
	public:
		MouseMoveEvent(float x, float y)
			: x(x), y(y) {
		}

		EventID getTypeID() const override {
			return typeid(MouseMoveEvent);
		}

		float getX() const { return x; }
		float getY() const { return y; }
	private:
		float x;
		float y;
	};
}

#endif // !MOUSE_MOVE_EVENT_H
