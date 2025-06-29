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
    }

	void GLWindow::shutdown()
	{
		if (_pWindow) {
			glfwDestroyWindow(this->_pWindow);
			_pWindow = nullptr;
		}
		glfwTerminate();
	}

	GLWindow::~GLWindow()
	{
		shutdown();
	}

	void GLWindow::onUpdate() {
		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
	}

	GLWindow::ErrorCode GLWindow::init(void** proc)
	{
		if (!s_glfwInit) {
			if (!glfwInit()) {
				TripleLogger::TLogger::Critical("Error Init GLFW!");
				return ErrorCode::GlfwInitError;
			}
			else {
				TripleLogger::TLogger::Info("GLFW initialized successfully.");
				s_glfwInit = true;
			}
		}

		_pWindow = glfwCreateWindow(this->_data.width, this->_data.height, this->_data.title, nullptr, nullptr);
		if (!_pWindow) {
			TripleLogger::TLogger::Critical("Failed to create GLFW window");
			glfwTerminate();
			return ErrorCode::CreateWindowError;
		}
		TripleLogger::TLogger::Info("GLFW window created successfully");

		glfwMakeContextCurrent(_pWindow);

		*proc = glfwGetProcAddress;

		glfwSetWindowUserPointer(_pWindow, &this->_data);

		glfwSetWindowSizeCallback(_pWindow, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.width = width;
			data.height = height;
			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(_pWindow, [](GLFWwindow* window) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			WindowCloseEvent event(data.title);
			data.eventCallback(event);
		});

		return ErrorCode::None;
	}
}
