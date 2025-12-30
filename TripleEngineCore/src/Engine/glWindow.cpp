#include "Engine/GlWindow.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Engine/EventSystem.h"
#include <TLogger.h>
#include "Engine/EventSystem.h"

namespace TripleEngineCore {

	static Event::KeyCode ConvertKey(int glfwKey)
	{
		switch (glfwKey)
		{
		case GLFW_KEY_W: return Event::KeyCode::W;
		case GLFW_KEY_A: return Event::KeyCode::A;
		case GLFW_KEY_S: return Event::KeyCode::S;
		case GLFW_KEY_D: return Event::KeyCode::D;
		case GLFW_KEY_SPACE: return Event::KeyCode::Space;
		case GLFW_KEY_ESCAPE: return Event::KeyCode::Escape;
		case GLFW_KEY_LEFT_SHIFT: return Event::KeyCode::LeftShift;
		case GLFW_KEY_LEFT_CONTROL: return Event::KeyCode::LeftCtrl;
		case GLFW_KEY_LEFT: return Event::KeyCode::Left;
		case GLFW_KEY_RIGHT: return Event::KeyCode::Right;
		case GLFW_KEY_UP: return Event::KeyCode::Up;
		case GLFW_KEY_DOWN: return Event::KeyCode::Down;
		case GLFW_KEY_F1: return Event::KeyCode::F1;
		case GLFW_KEY_F2: return Event::KeyCode::F2;
		case GLFW_KEY_F3: return Event::KeyCode::F3;
		case GLFW_KEY_F4: return Event::KeyCode::F4;
		case GLFW_KEY_F5: return Event::KeyCode::F5;
		case GLFW_KEY_F6: return Event::KeyCode::F6;
		case GLFW_KEY_F7: return Event::KeyCode::F7;
		case GLFW_KEY_F8: return Event::KeyCode::F8;
		case GLFW_KEY_F9: return Event::KeyCode::F9;
		case GLFW_KEY_F10: return Event::KeyCode::F10;
		case GLFW_KEY_F11: return Event::KeyCode::F11;
		case GLFW_KEY_F12: return Event::KeyCode::F12;
		default: return Event::KeyCode::Unknown;
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

	static Event::MouseButton ConvertMouseButton(int button)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:   return Event::MouseButton::Left;
		case GLFW_MOUSE_BUTTON_RIGHT:  return Event::MouseButton::Right;
		case GLFW_MOUSE_BUTTON_MIDDLE: return Event::MouseButton::Middle;
		case GLFW_MOUSE_BUTTON_4:      return Event::MouseButton::Button4;
		case GLFW_MOUSE_BUTTON_5:      return Event::MouseButton::Button5;
		default:                       return Event::MouseButton::Unknown;
		}
	}

	static bool s_glfwInit = false;
    GLWindow::GLWindow(const char* title, int width, int height)
    {
		_data.title = const_cast<char*>(title);
		_data.width = width;
		_data.height = height;
		this->_pWindow = nullptr;
    }

	GLWindow::ErrorCode GLWindow::init(void** proc)
	{
		if (!s_glfwInit) {
			if (!glfwInit()) {
				TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error Init GLFW!");
				return ErrorCode::GlfwInitError;
			}
			else {
				TripleLogger::TLogger::ModuleInfo(this->getModuleName(), "GLFW initialized successfully.");
				s_glfwInit = true;
			}
		}

		this->_pWindow = glfwCreateWindow(this->_data.width, this->_data.height, this->_data.title, nullptr, nullptr);
		if (!this->_pWindow) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to create GLFW window");
			shutdown();
			return ErrorCode::CreateWindowError;
		}
		else {
			glfwMakeContextCurrent(this->_pWindow);
			TripleLogger::TLogger::ModuleInfo(this->getModuleName(), "GLFW window created successfully");
		}

		glfwSetInputMode(this->_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		*proc = glfwGetProcAddress;
		glfwSetWindowUserPointer(this->_pWindow, &this->_data);
		this->initGLFWCallbacks();

		return ErrorCode::None;
	}

	void GLWindow::initGLFWCallbacks()
	{
		glfwSetWindowSizeCallback(this->_pWindow, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.width = width;
			data.height = height;
			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(this->_pWindow, [](GLFWwindow* window) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			WindowCloseEvent event(data.title);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(this->_pWindow, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			MouseMoveEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
			data.eventCallback(event);
		});

		glfwSetKeyCallback(this->_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			KeyboardInputEvent event(
				ConvertKey(key),
				ConvertAction(action),
				mods
			);

			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(this->_pWindow, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseButtonEvent event(
				ConvertMouseButton(button),
				ConvertAction(action),
				mods
			);

			data.eventCallback(event);
		});
	}

	void GLWindow::onUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(_pWindow);
	}

	void GLWindow::setFullscreen()
	{
		if (!_pWindow || _data.isFullscreen) return;

		glfwGetWindowPos(_pWindow, &_data.windowedX, &_data.windowedY);
		glfwGetWindowSize(_pWindow, &_data.windowedWidth, &_data.windowedHeight);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(_pWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		_data.isFullscreen = true;
	}

	void GLWindow::setWindowed()
	{
		if (!_pWindow || !_data.isFullscreen) return;

		glfwSetWindowMonitor(_pWindow, nullptr, _data.windowedX, _data.windowedY, _data.windowedWidth, _data.windowedHeight, 0);
		_data.isFullscreen = false;
	}

	void GLWindow::toggleFullscreen()
	{
		if (_data.isFullscreen)
			setWindowed();
		else
			setFullscreen();
	}

	double GLWindow::getTime() const
	{
		return glfwGetTime();
	}

	void GLWindow::shutdown()
	{
		if (s_glfwInit) {
			if (_pWindow) {
				glfwDestroyWindow(this->_pWindow);
				_pWindow = nullptr;
			}
			glfwTerminate();
		}
	}

	GLWindow::~GLWindow()
	{
		shutdown();
	}
}
