#include "triple/core/platform/GLWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <triple/log/Logger.h>

#include "triple/core/input/KeyCode.h"
#include "triple/core/input/MouseButton.h"

#include "triple/core/event/KeyAction.h"
#include "triple/core/event/KeyboardInputEvent.h"
#include "triple/core/event/MouseButtonEvent.h"
#include "triple/core/event/MouseMoveEvent.h"
#include "triple/core/event/TextInputEvent.h"
#include "triple/core/event/WindowCloseEvent.h"
#include "triple/core/event/WindowResizeEvent.h"

namespace triple::core {

	static KeyCode convertKey(int glfwKey) {
		if (glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_Z)
			return static_cast<KeyCode>(static_cast<int>(KeyCode::A) + (glfwKey - GLFW_KEY_A));
		if (glfwKey >= GLFW_KEY_0 && glfwKey <= GLFW_KEY_9)
			return static_cast<KeyCode>(static_cast<int>(KeyCode::Num0) + (glfwKey - GLFW_KEY_0));
		if (glfwKey >= GLFW_KEY_F1 && glfwKey <= GLFW_KEY_F25)
			return static_cast<KeyCode>(static_cast<int>(KeyCode::F1) + (glfwKey - GLFW_KEY_F1));
		if (glfwKey >= GLFW_KEY_KP_0 && glfwKey <= GLFW_KEY_KP_9)
			return static_cast<KeyCode>(static_cast<int>(KeyCode::KP0) + (glfwKey - GLFW_KEY_KP_0));

		switch (glfwKey) {
		case GLFW_KEY_LEFT:
			return KeyCode::Left;
		case GLFW_KEY_RIGHT:
			return KeyCode::Right;
		case GLFW_KEY_UP:
			return KeyCode::Up;
		case GLFW_KEY_DOWN:
			return KeyCode::Down;
		case GLFW_KEY_LEFT_SHIFT:
			return KeyCode::LeftShift;
		case GLFW_KEY_RIGHT_SHIFT:
			return KeyCode::RightShift;
		case GLFW_KEY_LEFT_CONTROL:
			return KeyCode::LeftCtrl;
		case GLFW_KEY_RIGHT_CONTROL:
			return KeyCode::RightCtrl;
		case GLFW_KEY_LEFT_ALT:
			return KeyCode::LeftAlt;
		case GLFW_KEY_RIGHT_ALT:
			return KeyCode::RightAlt;
		case GLFW_KEY_LEFT_SUPER:
			return KeyCode::LeftSuper;
		case GLFW_KEY_RIGHT_SUPER:
			return KeyCode::RightSuper;
		case GLFW_KEY_INSERT:
			return KeyCode::Insert;
		case GLFW_KEY_DELETE:
			return KeyCode::Delete;
		case GLFW_KEY_HOME:
			return KeyCode::Home;
		case GLFW_KEY_END:
			return KeyCode::End;
		case GLFW_KEY_PAGE_UP:
			return KeyCode::PageUp;
		case GLFW_KEY_PAGE_DOWN:
			return KeyCode::PageDown;
		case GLFW_KEY_SPACE:
			return KeyCode::Space;
		case GLFW_KEY_TAB:
			return KeyCode::Tab;
		case GLFW_KEY_BACKSPACE:
			return KeyCode::Backspace;
		case GLFW_KEY_ENTER:
			return KeyCode::Enter;
		case GLFW_KEY_ESCAPE:
			return KeyCode::Escape;
		case GLFW_KEY_CAPS_LOCK:
			return KeyCode::CapsLock;
		case GLFW_KEY_PRINT_SCREEN:
			return KeyCode::PrintScreen;
		case GLFW_KEY_SCROLL_LOCK:
			return KeyCode::ScrollLock;
		case GLFW_KEY_PAUSE:
			return KeyCode::Pause;
		case GLFW_KEY_MENU:
			return KeyCode::Menu;
		case GLFW_KEY_NUM_LOCK:
			return KeyCode::NumLock;
		case GLFW_KEY_KP_DECIMAL:
			return KeyCode::KPDecimal;
		case GLFW_KEY_KP_DIVIDE:
			return KeyCode::KPDivide;
		case GLFW_KEY_KP_MULTIPLY:
			return KeyCode::KPMultiply;
		case GLFW_KEY_KP_SUBTRACT:
			return KeyCode::KPSubtract;
		case GLFW_KEY_KP_ADD:
			return KeyCode::KPAdd;
		case GLFW_KEY_KP_ENTER:
			return KeyCode::KPEnter;
		case GLFW_KEY_KP_EQUAL:
			return KeyCode::KPEqual;
		case GLFW_KEY_APOSTROPHE:
			return KeyCode::Apostrophe;
		case GLFW_KEY_COMMA:
			return KeyCode::Comma;
		case GLFW_KEY_MINUS:
			return KeyCode::Minus;
		case GLFW_KEY_PERIOD:
			return KeyCode::Period;
		case GLFW_KEY_SLASH:
			return KeyCode::Slash;
		case GLFW_KEY_SEMICOLON:
			return KeyCode::Semicolon;
		case GLFW_KEY_EQUAL:
			return KeyCode::Equal;
		case GLFW_KEY_LEFT_BRACKET:
			return KeyCode::LeftBracket;
		case GLFW_KEY_BACKSLASH:
			return KeyCode::Backslash;
		case GLFW_KEY_RIGHT_BRACKET:
			return KeyCode::RightBracket;
		case GLFW_KEY_GRAVE_ACCENT:
			return KeyCode::GraveAccent;
		case GLFW_KEY_WORLD_1:
			return KeyCode::World1;
		case GLFW_KEY_WORLD_2:
			return KeyCode::World2;
		default:
			return KeyCode::Unknown;
		}
	}

	static KeyAction convertAction(int glfwAction) {
		switch (glfwAction) {
		case GLFW_PRESS:
			return KeyAction::Press;
		case GLFW_RELEASE:
			return KeyAction::Release;
		case GLFW_REPEAT:
			return KeyAction::Repeat;
		default:
			return KeyAction::Release;
		}
	}

	static MouseButton convertMouseButton(int button) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			return MouseButton::Left;
		case GLFW_MOUSE_BUTTON_RIGHT:
			return MouseButton::Right;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return MouseButton::Middle;
		case GLFW_MOUSE_BUTTON_4:
			return MouseButton::Button4;
		case GLFW_MOUSE_BUTTON_5:
			return MouseButton::Button5;
		default:
			return MouseButton::Unknown;
		}
	}

	static bool sGlfwInit = false;
	GLWindow::GLWindow(const char *title, int width, int height, IEventSink *sink)
	    : m_window(nullptr) {
		m_data.title = const_cast<char *>(title);
		m_data.width = width;
		m_data.height = height;
		m_data.eventSink = sink;
	}

	GLWindow::ErrorCode GLWindow::init(void **proc) {
		if (!sGlfwInit) {
			if (!glfwInit()) {
				triple::log::Logger::ModuleCritical("GLWindow", "Error Init GLFW!");
				return ErrorCode::GlfwInitError;
			} else {
				triple::log::Logger::ModuleInfo("GLWindow", "GLFW initialized successfully.");
				sGlfwInit = true;
			}
		}

		this->m_window = glfwCreateWindow(this->m_data.width, this->m_data.height,
		                                  this->m_data.title, nullptr, nullptr);
		if (!this->m_window) {
			triple::log::Logger::ModuleCritical("GLWindow", "Failed to create GLFW window");
			shutdown();
			return ErrorCode::CreateWindowError;
		} else {
			glfwMakeContextCurrent(this->m_window);
			triple::log::Logger::ModuleInfo("GLWindow", "GLFW window created successfully");
		}

		*proc = reinterpret_cast<void *>(glfwGetProcAddress);
		glfwSetWindowUserPointer(this->m_window, &this->m_data);
		this->initGLFWCallbacks();

		return ErrorCode::None;
	}

	void GLWindow::pollEvents() { glfwPollEvents(); }

	void GLWindow::swapBuffers() { glfwSwapBuffers(this->m_window); }

	bool GLWindow::shouldClose() const { return glfwWindowShouldClose(this->m_window); }

	void *GLWindow::getNativeWindow() const { return this->m_window; }

	bool GLWindow::isFullscreen() const { return this->m_data.isFullscreen; }

	void GLWindow::setFullscreen(bool enabled) {
		if (enabled) {
			if (!m_window || m_data.isFullscreen)
				return;

			glfwGetWindowPos(m_window, &m_data.windowedX, &m_data.windowedY);
			glfwGetWindowSize(m_window, &m_data.windowedWidth, &m_data.windowedHeight);

			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height,
			                     mode->refreshRate);
			m_data.isFullscreen = true;
		} else {
			if (!m_window || !m_data.isFullscreen)
				return;

			glfwSetWindowMonitor(m_window, nullptr, m_data.windowedX, m_data.windowedY,
			                     m_data.windowedWidth, m_data.windowedHeight, 0);
			m_data.isFullscreen = false;
		}
	}

	void GLWindow::setSize(uint32_t width, uint32_t height) {
		if (!m_window)
			return;

		if (m_data.isFullscreen)
			setFullscreen(false);

		glfwSetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
	}

	void GLWindow::setPosition(uint32_t x, uint32_t y) {
		if (!m_window)
			return;

		glfwSetWindowPos(m_window, static_cast<int>(x), static_cast<int>(y));
	}

	float GLWindow::getDPIScale() const {
		if (!m_window)
			return 1.0f;

		float xscale = 1.0f;
		float yscale = 1.0f;

		glfwGetWindowContentScale(m_window, &xscale, &yscale);

		return xscale;
	}

	void GLWindow::initGLFWCallbacks() {
		glfwSetWindowSizeCallback(this->m_window, [](GLFWwindow *window, int width, int height) {
			WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
			data.width = width;
			data.height = height;
			WindowResizeEvent event(width, height);
			data.eventSink->pushEvent(event);
		});

		glfwSetWindowCloseCallback(this->m_window, [](GLFWwindow *window) {
			WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
			WindowCloseEvent event(data.title);
			data.eventSink->pushEvent(event);
		});

		glfwSetCursorPosCallback(this->m_window, [](GLFWwindow *window, double xpos, double ypos) {
			WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
			MouseMoveEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
			data.eventSink->pushEvent(event);
		});

		glfwSetKeyCallback(
		    this->m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
			    WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

			    KeyboardInputEvent event(convertKey(key), convertAction(action), mods);

			    data.eventSink->pushEvent(event);
		    });

		glfwSetMouseButtonCallback(
		    this->m_window, [](GLFWwindow *window, int button, int action, int mods) {
			    WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

			    MouseButtonEvent event(convertMouseButton(button), convertAction(action), mods);

			    data.eventSink->pushEvent(event);
		    });

		glfwSetCharCallback(this->m_window, [](GLFWwindow *window, unsigned int codepoint) {
			WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
			TextInputEvent event(codepoint);
			data.eventSink->pushEvent(event);
		});
	}

	void GLWindow::setCursorCapture(bool capture) {
		if (capture) {
			glfwSetInputMode(this->m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_data.isCursorCaptured = true;
		} else {
			glfwSetInputMode(this->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_data.isCursorCaptured = false;
		}
	}

	bool GLWindow::isCursorCaptured() const { return m_data.isCursorCaptured; }

	double GLWindow::getTime() const { return glfwGetTime(); }

	void GLWindow::shutdown() {
		if (sGlfwInit) {
			if (m_window) {
				glfwDestroyWindow(this->m_window);
				m_window = nullptr;
			}
			glfwTerminate();
		}
	}

	int GLWindow::getWidth() {
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
		return w;
	}

	int GLWindow::getHeight() {
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
		return h;
	}

	GLWindow::~GLWindow() { shutdown(); }
} // namespace triple::core
