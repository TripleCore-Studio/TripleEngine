#ifndef IWINDOW_H
#define IWINDOW_H

#include <cstdint>

namespace triple::core {
	class IWindow {
	public:
		virtual ~IWindow() = default;
		virtual void pollEvents() = 0;
		virtual void swapBuffers() = 0;
		[[nodiscard]] virtual bool shouldClose() const = 0;
		[[nodiscard]] virtual void *getNativeWindow() const = 0;
		[[nodiscard]] virtual bool isFullscreen() const = 0;
		virtual void setFullscreen(bool enabled) = 0;
		virtual void setSize(uint32_t width, uint32_t height) = 0;
		virtual void setPosition(uint32_t x, uint32_t y) = 0;
		[[nodiscard]] virtual float getDPIScale() const = 0;
		virtual void setCursorCapture(bool capture) = 0;
		[[nodiscard]] virtual bool isCursorCaptured() const = 0;
		[[nodiscard]] virtual double getTime() const = 0;
		virtual void shutdown() = 0;
		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
	};
} // namespace triple::core

#endif // IWINDOW_H
