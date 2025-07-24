#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <functional>
#include <unordered_map>
#include <ExportMacros.h>

namespace TripleEngineCore {
	class Event {
	public:
		enum class Type
		{
			None = 0,
			WindowClose,
			WindowResize,
			MouseMove
		};

		virtual Type getType() const = 0;
		virtual ~Event() = default;
	};

	class MouseMoveEvent : public Event {
	public:
		MouseMoveEvent(float x, float y)
			: x(x), y(y) {
		}
		Type getType() const override { return Type::MouseMove; }
		float getX() const { return x; }
		float getY() const { return y; }
	private:
		float x;
		float y;
	};

	class WindowCloseEvent : public Event {
	public:
		WindowCloseEvent(const char* title)
			: _title(const_cast<char*>(title)) {
		}
		Type getType() const override { return Type::WindowClose; };
		const char* getTitle() const { return _title; }
	private:
		char* _title;
	};

	class WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(int width, int height)
			: width(width), height(height) {
		}
		Type getType() const override { return Type::WindowResize; }
		int getWidth() const { return width; }
		int getHeight() const { return height; };
	private:
		int width;
		int height;
	};

	class CORE_API EventDispatcher {
	public:
		EventDispatcher() = default;
		using EventCallbackFn = std::function<void(Event&)>;
		void addListener(Event::Type type, const EventCallbackFn callback);
		void dispatch(Event& event);
	private:
		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher(EventDispatcher&&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;
		EventDispatcher& operator=(EventDispatcher&&) = delete;

		std::unordered_map<Event::Type, std::vector<EventCallbackFn>> listeners;
	};
}
#endif // EVENT_DISPATCHER_H