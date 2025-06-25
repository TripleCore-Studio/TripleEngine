#include <Application.h>
#include "GlWindow.h"
#include <TLogger.h>

namespace TripleEngineCore {
	Application::Application()
	{
		TripleLogger::TLogger::Instance().LogInfo("Application starting...");
	}

	Application::~Application()
	{
		TripleLogger::TLogger::Instance().LogInfo("Application stoping...");
	}

	int Application::start(const char* title, unsigned int width, unsigned int height)
	{
		this->_pWindow = std::make_unique<GLWindow>(title, width, height);

		while (true) {
			this->onUpdate();
			this->_pWindow->onUpdate();
		}
	}

	void Application::onUpdate()
	{

	}
}