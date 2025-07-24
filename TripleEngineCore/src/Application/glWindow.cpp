#include "Application/GlWindow.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Application/EventSystem.h"
#include <TLogger.h>

namespace TripleEngineCore {
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
	}

	void GLWindow::onUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(_pWindow);
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
