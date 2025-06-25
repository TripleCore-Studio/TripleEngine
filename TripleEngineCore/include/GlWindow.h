#ifndef GLWINDOW_H
#define GLWINDOW_H

#ifdef _WIN32
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif

struct GLFWwindow;

namespace TripleEngineCore {
	class CORE_API GLWindow {
	public:
		GLWindow(const char* title, int width, int height);
		~GLWindow();
		virtual void shutdown();
		virtual void onUpdate();

		enum class ErrorCode {
			None = 0,
			GlfwInitError,
			CreateWindowError
		};
	private:
		ErrorCode init();
		GLWindow(const GLWindow&) = delete;
		GLWindow& operator=(const GLWindow&) = delete;

		struct WindowData
		{
		public:
			char* title;
			int width;
			int height;
		};

		WindowData _data;
		GLFWwindow* _pWindow;
	};
}
#endif // GLWINDOW_H