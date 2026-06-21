#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <triple/core/base/ExportMacros.h>
#include "triple/core/base/IWindow.h"

#include "triple/core/event/Event.h"
#include "triple/core/event/IEventSink.h"

struct GLFWwindow;

namespace triple::core {
	class CORE_API GLWindow : public IWindow {
	public:
		enum class ErrorCode { None = 0, GlfwInitError, CreateWindowError };

		GLWindow(const char *title, int width, int height, IEventSink *sink);
		ErrorCode init(void **outProc);

		void pollEvents() override;
		void swapBuffers() override;
		bool shouldClose() const override;
		void *getNativeWindow() const override;
		bool isFullscreen() const override;
		void setFullscreen(bool enabled) override;
		void setSize(uint32_t width, uint32_t height) override;
		void setPosition(uint32_t x, uint32_t y) override;
		float getDPIScale() const override;
		void setCursorCapture(bool capture) override;
		bool isCursorCaptured() const override;
		double getTime() const override;
		void shutdown() override;
		int getWidth() override;
		int getHeight() override;

		~GLWindow();

	private:
		GLWindow(const GLWindow &) = delete;
		GLWindow &operator=(const GLWindow &) = delete;
		void initGLFWCallbacks();
		void emit(Event &e);

		struct WindowData {
		public:
			char *title = nullptr;
			int width = 0;
			int height = 0;

			int windowedX = 100;
			int windowedY = 100;
			int windowedWidth = 800;
			int windowedHeight = 600;
			bool isFullscreen = false;
			bool isCursorCaptured = false;

			IEventSink *eventSink = nullptr;
		};

		WindowData m_data;
		GLFWwindow *m_window;
	};
} // namespace triple::core
#endif // GLWINDOW_H