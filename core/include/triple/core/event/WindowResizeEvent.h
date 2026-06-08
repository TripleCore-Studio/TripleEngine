#ifndef WINDOW_RESIZE_EVENT_H
#define WINDOW_RESIZE_EVENT_H

#include "EventBase.h"

namespace triple::core {
	class CORE_API WindowResizeEvent : public EventBase<WindowResizeEvent> {
	public:
		WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}

		[[nodiscard]] int getWidth() const { return m_width; }
		[[nodiscard]] int getHeight() const { return m_height; };

	private:
		int m_width;
		int m_height;
	};
} // namespace triple::core

#endif // WINDOW_RESIZE_EVENT_H
