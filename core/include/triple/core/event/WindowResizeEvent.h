#ifndef WINDOW_RESIZE_EVENT_H
#define WINDOW_RESIZE_EVENT_H

#include "Event.h"

namespace triple::core {
	class CORE_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(int width, int height)
			: m_width(width), m_height(height) {
		}

		EventID getTypeID() const override {
			return typeid(WindowResizeEvent);
		}

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; };
	private:
		int m_width;
		int m_height;
	};
}

#endif // WINDOW_RESIZE_EVENT_H
