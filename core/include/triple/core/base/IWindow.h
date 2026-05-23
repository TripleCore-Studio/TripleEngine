#ifndef IWINDOW_H
#define IWINDOW_H

#include <cstdint>

namespace triple::core {
	class IWindow {
	public:
		virtual ~IWindow() = default;
		virtual void PollEvents() = 0;
		virtual void SwapBuffers() = 0;
		virtual bool ShouldClose() const = 0;
		virtual void* GetNativeWindow() const = 0;
		virtual bool isFullscreen() const = 0;
		virtual void setFullscreen(bool enabled) = 0;
		virtual void setSize(uint32_t width, uint32_t height) = 0;
		virtual void setPosition(uint32_t x, uint32_t y) = 0;
		virtual float getDPIScale() const = 0;
		virtual void setCursorCapture(bool capture) = 0;
		virtual bool isCursorCaptured() const = 0;
		virtual double getTime() const = 0;
		virtual void shutdown() = 0;
	};
}

#endif // IWINDOW_H
