#include "Engine/GLWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <TLogger.h>

#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

#include "Event/KeyAction.h"

#include "Event/KeyboardInputEvent.h"
#include "Event/MouseButtonEvent.h"
#include "Event/MouseMoveEvent.h"
#include "Event/WindowCloseEvent.h"
#include "Event/WindowResizeEvent.h"

namespace TripleEngineCore {

	static Input::KeyCode ConvertKey(int glfwKey)
	{
		switch (glfwKey)
		{
		case GLFW_KEY_W: return Input::KeyCode::W;
		case GLFW_KEY_A: return Input::KeyCode::A;
		case GLFW_KEY_S: return Input::KeyCode::S;
		case GLFW_KEY_D: return Input::KeyCode::D;
		case GLFW_KEY_SPACE: return Input::KeyCode::Space;
		case GLFW_KEY_ESCAPE: return Input::KeyCode::Escape;
		case GLFW_KEY_LEFT_SHIFT: return Input::KeyCode::LeftShift;
		case GLFW_KEY_LEFT_CONTROL: return Input::KeyCode::LeftCtrl;
		case GLFW_KEY_LEFT: return Input::KeyCode::Left;
		case GLFW_KEY_RIGHT: return Input::KeyCode::Right;
		case GLFW_KEY_UP: return Input::KeyCode::Up;
		case GLFW_KEY_DOWN: return Input::KeyCode::Down;
		case GLFW_KEY_F1: return Input::KeyCode::F1;
		case GLFW_KEY_F2: return Input::KeyCode::F2;
		case GLFW_KEY_F3: return Input::KeyCode::F3;
		case GLFW_KEY_F4: return Input::KeyCode::F4;
		case GLFW_KEY_F5: return Input::KeyCode::F5;
		case GLFW_KEY_F6: return Input::KeyCode::F6;
		case GLFW_KEY_F7: return Input::KeyCode::F7;
		case GLFW_KEY_F8: return Input::KeyCode::F8;
		case GLFW_KEY_F9: return Input::KeyCode::F9;
		case GLFW_KEY_F10: return Input::KeyCode::F10;
		case GLFW_KEY_F11: return Input::KeyCode::F11;
		case GLFW_KEY_F12: return Input::KeyCode::F12;
		default: return Input::KeyCode::Unknown;
		}
	}

	static Event::KeyAction ConvertAction(int glfwAction)
	{
		switch (glfwAction)
		{
		case GLFW_PRESS:   return Event::KeyAction::Press;
		case GLFW_RELEASE: return Event::KeyAction::Release;
		case GLFW_REPEAT:  return Event::KeyAction::Repeat;
		default:           return Event::KeyAction::Release;
		}
	}

	static Input::MouseButton ConvertMouseButton(int button)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:   return Input::MouseButton::Left;
		case GLFW_MOUSE_BUTTON_RIGHT:  return Input::MouseButton::Right;
		case GLFW_MOUSE_BUTTON_MIDDLE: return Input::MouseButton::Middle;
		case GLFW_MOUSE_BUTTON_4:      return Input::MouseButton::Button4;
		case GLFW_MOUSE_BUTTON_5:      return Input::MouseButton::Button5;
		default:                       return Input::MouseButton::Unknown;
		}
	}

	static bool s_glfwInit = false;
    GLWindow::GLWindow(const char* title, int width, int height, IEventSink* sink)
    {
		m_data.title = const_cast<char*>(title);
		m_data.width = width;
		m_data.height = height;
		m_data.eventSink = sink;
		this->m_window = nullptr;
    }

	GLWindow::ErrorCode GLWindow::init(void** proc)
	{
		if (!s_glfwInit) {
			if (!glfwInit()) {
				TripleLogger::TLogger::ModuleCritical("GLWindow", "Error Init GLFW!");
				return ErrorCode::GlfwInitError;
			}
			else {
				TripleLogger::TLogger::ModuleInfo("GLWindow", "GLFW initialized successfully.");
				s_glfwInit = true;
			}
		}

		this->m_window = glfwCreateWindow(this->m_data.width, this->m_data.height, this->m_data.title, nullptr, nullptr);
		if (!this->m_window) {
			TripleLogger::TLogger::ModuleCritical("GLWindow", "Failed to create GLFW window");
			shutdown();
			return ErrorCode::CreateWindowError;
		}
		else {
			glfwMakeContextCurrent(this->m_window);
			TripleLogger::TLogger::ModuleInfo("GLWindow", "GLFW window created successfully");
		}

		*proc = reinterpret_cast<void*>(glfwGetProcAddress);
		glfwSetWindowUserPointer(this->m_window, &this->m_data);
		this->initGLFWCallbacks();

		return ErrorCode::None;
	}

	void GLWindow::PollEvents()
	{
		glfwPollEvents();
	}

	void GLWindow::SwapBuffers()
	{
		glfwSwapBuffers(this->m_window);
	}

	bool GLWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(this->m_window);
	}

	void* GLWindow::GetNativeWindow() const
	{
		return this->m_window;
	}

	bool GLWindow::isFullscreen() const
	{
		return this->m_data.isFullscreen;
	}

	void GLWindow::setFullscreen(bool enabled)
	{
		if (enabled) {
			if (!m_window || m_data.isFullscreen) return;

			glfwGetWindowPos(m_window, &m_data.windowedX, &m_data.windowedY);
			glfwGetWindowSize(m_window, &m_data.windowedWidth, &m_data.windowedHeight);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			m_data.isFullscreen = true;
		}
		else {
			if (!m_window || !m_data.isFullscreen) return;

			glfwSetWindowMonitor(m_window, nullptr, m_data.windowedX, m_data.windowedY, m_data.windowedWidth, m_data.windowedHeight, 0);
			m_data.isFullscreen = false;
		}
	}

	void GLWindow::setSize(uint32_t width, uint32_t height)
	{
		if (!m_window)
			return;

		if (m_data.isFullscreen)
			setFullscreen(false);

		glfwSetWindowSize(m_window,
			static_cast<int>(width),
			static_cast<int>(height)
		);
	}

	void GLWindow::setPosition(uint32_t x, uint32_t y)
	{
		if (!m_window)
			return;

		glfwSetWindowPos(m_window,
			static_cast<int>(x),
			static_cast<int>(y)
		);
	}

	float GLWindow::getDPIScale() const
	{
		if (!m_window)
			return 1.0f;

		float xscale = 1.0f;
		float yscale = 1.0f;

		glfwGetWindowContentScale(m_window, &xscale, &yscale);

		return xscale;
	}

	void GLWindow::initGLFWCallbacks()
	{
		glfwSetWindowSizeCallback(this->m_window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.width = width;
			data.height = height;
			Event::WindowResizeEvent event(width, height);
			data.eventSink->pushEvent(event);
		});

		glfwSetWindowCloseCallback(this->m_window, [](GLFWwindow* window) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			Event::WindowCloseEvent event(data.title);
			data.eventSink->pushEvent(event);
		});

		glfwSetCursorPosCallback(this->m_window, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			Event::MouseMoveEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
			data.eventSink->pushEvent(event);
		});

		glfwSetKeyCallback(this->m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			Event::KeyboardInputEvent event(
				ConvertKey(key),
				ConvertAction(action),
				mods
			);

			data.eventSink->pushEvent(event);
		});

		glfwSetMouseButtonCallback(this->m_window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			Event::MouseButtonEvent event(
				ConvertMouseButton(button),
				ConvertAction(action),
				mods
			);

			data.eventSink->pushEvent(event);
		});
	}

	void GLWindow::emit(Event::Event& e)
	{
		if (m_data.eventSink)
			m_data.eventSink->pushEvent(e);
	}

	void GLWindow::setCursorCapture(bool capture)
	{
		if (capture) {
			glfwSetInputMode(this->m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_data.isCursorCaptured = true;
		}
		else {
			glfwSetInputMode(this->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_data.isCursorCaptured = false;
		}
	}

	bool GLWindow::isCursorCaptured() const
	{
		return m_data.isCursorCaptured;
	}

	double GLWindow::getTime() const
	{
		return glfwGetTime();
	}

	void GLWindow::shutdown()
	{
		if (s_glfwInit) {
			if (m_window) {
				glfwDestroyWindow(this->m_window);
				m_window = nullptr;
			}
			glfwTerminate();
		}
	}

	GLWindow::~GLWindow()
	{
		shutdown();
	}
}
