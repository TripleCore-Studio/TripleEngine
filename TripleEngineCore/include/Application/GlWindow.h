#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <memory>
#include <functional>
#include <ExportMacros.h>

struct GLFWwindow;

namespace TripleEngineCore {
	class CORE_API GLWindow {
	public:
		enum class ErrorCode {
			None = 0,
			GlfwInitError,
			CreateWindowError
		};

		using EventCallbackFn = std::function<void(class Event&)>;

		GLWindow(const char* title, int width, int height);
		~GLWindow();

		void setEventCallback(const EventCallbackFn& callback) { _data.eventCallback = callback; }

		virtual void shutdown();
		virtual void onUpdate();

		ErrorCode init(void** outProc);
	private:
		GLWindow(const GLWindow&) = delete;
		GLWindow& operator=(const GLWindow&) = delete;

		struct WindowData
		{
		public:
			char* title;
			int width;
			int height;
			EventCallbackFn eventCallback;
		};

		WindowData _data;
		GLFWwindow* _pWindow;
	};
}
#endif // GLWINDOW_H