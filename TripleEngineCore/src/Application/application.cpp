#include "Application/Application.h"
#include "Application/GlWindow.h"
#include "Application/EventSystem.h"
#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "Interfaces/IRenderer.h"
#include "TLogger.h"

namespace TripleEngineCore {
	Application::Application()
	{
		TripleLogger::TLogger::Info("Application starting...");
		this->_pEventDispatcher = std::make_unique<EventDispatcher>();

		this->_pModuleLoader = std::make_unique<System::ModuleLoader>("modules");
		this->_pModuleLoader->loadModule(ModuleType::OpenGLRenderer);

		this->_isRunning = true;
	}

	Application::~Application()
	{
		TripleLogger::TLogger::Info("Application stoping...");
	}

	int Application::start(const char* title, unsigned int width, unsigned int height)
	{
		this->_pWindow = std::make_unique<GLWindow>(title, width, height);
		void* proc = nullptr;
		this->_pWindow->init(&proc);

		OpenGLRenderModule* module = dynamic_cast<OpenGLRenderModule*>(this->_pModuleLoader->getModule(ModuleType::OpenGLRenderer));
		IRenderer* rend = module->getRenderer();
		IOpenGLRenderer* glRend = dynamic_cast<IOpenGLRenderer*>(rend);

		glRend->initGlad(proc);

		this->loadCallbacks();

		while (_isRunning) {
			glRend->RenderFrame();
			this->onUpdate();
			this->_pWindow->onUpdate();
		}

		return 0;
	}

	void Application::onUpdate()
	{

	}

	void Application::loadCallbacks()
	{
		this->_pWindow->setEventCallback([this](Event& event) {
			this->_pEventDispatcher->dispatch(event);
			});

		this->_pEventDispatcher->addListener(Event::Type::WindowClose, [this](Event& event) {
			if (event.getType() == Event::Type::WindowClose) {
				auto& closeEvent = static_cast<WindowCloseEvent&>(event);
				TripleLogger::TLogger::Warn("Window ({}) closed", closeEvent.getTitle());
				this->_pWindow->shutdown();
				this->_isRunning = false;
			}
			});

		this->_pEventDispatcher->addListener(Event::Type::WindowResize, [this](Event& event) {
			if (event.getType() == Event::Type::WindowResize) {
				auto& resizeEvent = static_cast<WindowResizeEvent&>(event);
				TripleLogger::TLogger::Info("Window resized to {}x{}", resizeEvent.getWidth(), resizeEvent.getHeight());
			}
			});
	}
}