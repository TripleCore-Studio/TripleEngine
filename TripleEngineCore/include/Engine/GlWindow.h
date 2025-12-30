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
		ErrorCode init(void** outProc);
		virtual void onUpdate();

		void setEventCallback(const EventCallbackFn& callback) { _data.eventCallback = callback; }

		void setFullscreen();
		void setWindowed();
		void toggleFullscreen();

		const char* getModuleName() const { return "GLWindow"; }
		double getTime() const;

		virtual void shutdown();
		~GLWindow();
	private:
		GLWindow(const GLWindow&) = delete;
		GLWindow& operator=(const GLWindow&) = delete;
		void initGLFWCallbacks();

		struct WindowData
		{
		public:
			char* title = nullptr;
			int width = 0;
			int height = 0;
			EventCallbackFn eventCallback = nullptr;

			int windowedX = 100;
			int windowedY = 100;
			int windowedWidth = 800;
			int windowedHeight = 600;
			bool isFullscreen = false;
		};

		WindowData _data;
		GLFWwindow* _pWindow;
	};
}
#endif // GLWINDOW_H