#include "GlWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <TLogger.h>

namespace TripleEngineCore {
	static bool s_glfwInit = false;
    GLWindow::GLWindow(const char* title, int width, int height)
    {
		_data.title = const_cast<char*>(title);
		_data.width = width;
		_data.height = height;
		GLWindow::ErrorCode resCode = init();
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
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
	}

	GLWindow::ErrorCode GLWindow::init()
	{
		if (!s_glfwInit) {
			if (!glfwInit()) {
				TripleLogger::TLogger::Instance().LogCritical("Error Init GLFW!");
				return ErrorCode::GlfwInitError;
			}
			else {
				TripleLogger::TLogger::Instance().LogInfo("GLFW initialized successfully.");
				s_glfwInit = true;
			}
		}

		_pWindow = glfwCreateWindow(this->_data.width, this->_data.height, this->_data.title, nullptr, nullptr);
		if (!_pWindow) {
			TripleLogger::TLogger::Instance().LogCritical("Failed to create GLFW window");
			glfwTerminate();
			return ErrorCode::CreateWindowError;
		}
		TripleLogger::TLogger::Instance().LogInfo("GLFW window created successfully");

		glfwMakeContextCurrent(_pWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			TripleLogger::TLogger::Instance().LogCritical("Failed to init GLAD");
			return ErrorCode::GlfwInitError;
		}

		return ErrorCode::None;
	}
}
