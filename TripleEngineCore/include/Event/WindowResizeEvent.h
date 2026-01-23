#ifndef WINDOW_RESIZE_EVENT_H
#define WINDOW_RESIZE_EVENT_H

#include "Event/Event.h"

namespace TripleEngineCore::Event {
	class CORE_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(int width, int height)
			: width(width), height(height) {
		}

		EventID getTypeID() const override {
			return typeid(WindowResizeEvent);
		}

		int getWidth() const { return width; }
		int getHeight() const { return height; };
	private:
		int width;
		int height;
	};
}

#endif // WINDOW_RESIZE_EVENT_H
