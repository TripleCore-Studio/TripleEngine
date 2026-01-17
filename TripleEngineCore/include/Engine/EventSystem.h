#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <functional>
#include <unordered_map>
#include <ExportMacros.h>

namespace TripleEngineCore {
	class CORE_API Event {
	public:
		enum class Type
		{
			None = 0,
			WindowClose,
			WindowResize,
			MouseMove,
			KeyboardInput,
			MouseButtonInput,
			EngineLoaded
		};

		enum class CORE_API KeyAction {
			Press,
			Release,
			Repeat
		};

		enum class CORE_API KeyCode : int {
			Unknown = -1,

			A, B, C, D, W, S,
			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			Space,
			Escape,
			LeftShift,
			RightShift,
			LeftCtrl,
			RightCtrl,
			Left,
			Right,
			Up,
			Down
		};

		enum class CORE_API MouseButton {
			Left = 0,
			Right = 1,
			Middle = 2,
			Button4,
			Button5,
			Unknown
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

	class KeyboardInputEvent : public Event {
	public:
		KeyboardInputEvent(KeyCode key, KeyAction action, int mods)
			: key(key), action(action), mods(mods) {
		}

		Type getType() const override { return Type::KeyboardInput; }

		KeyCode getKey() const { return key; }
		KeyAction getAction() const { return action; }
		int getMods() const { return mods; }

		bool isPressed() const { return action == KeyAction::Press; }
		bool isReleased() const { return action == KeyAction::Release; }
		bool isRepeat() const { return action == KeyAction::Repeat; }
	private:
		KeyCode key;
		KeyAction action;
		int mods;
	};

	class MouseButtonEvent : public Event {
	public:
		MouseButtonEvent(MouseButton button, KeyAction action, int mods)
			: button(button), action(action), mods(mods) {
		}

		Type getType() const override { return Type::MouseButtonInput; }

		MouseButton getButton() const { return button; }
		KeyAction getAction() const { return action; }
		int getMods() const { return mods; }

		bool isPressed() const { return action == KeyAction::Press; }
		bool isReleased() const { return action == KeyAction::Release; }
		bool isRepeat() const { return action == KeyAction::Repeat; }

	private:
		MouseButton button;
		KeyAction action;
		int mods;
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

	class CORE_API EngineLoadedEvent : public Event {
	public:
		EngineLoadedEvent(){}
		Type getType() const override { return Type::EngineLoaded; }
	};

	class CORE_API EventDispatcher {
	public:
		EventDispatcher() = default;
		using EventCallbackFn = std::function<void(Event&)>;
		void addListener(Event::Type type, const EventCallbackFn& callback);
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