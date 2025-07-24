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
		this->_isRunning = false;

		this->_pRenderer = nullptr;
	}

	Application::ErrorCode Application::start(const char* title, unsigned int width, unsigned int height)
	{
		OpenGLRenderModule* openGLModule = dynamic_cast<OpenGLRenderModule*>(this->_pModuleLoader->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		this->_pRenderer = openGLModule->getRenderer();
		if (this->_pRenderer == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}

		IOpenGLRenderer* pGLRenderer = dynamic_cast<IOpenGLRenderer*>(this->_pRenderer);
		if (!pGLRenderer) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		this->_pWindow = std::make_unique<GLWindow>(title, width, height);
		void* loader = nullptr;
		if (this->_pWindow->init(&loader) != GLWindow::ErrorCode::None) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}

		if (loader == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Loader for OpenGL not initialized");
			return ErrorCode::FailedToLoadWindow;
		}

		if (pGLRenderer->initGlad(loader)) {
			TripleLogger::TLogger::ModuleInfo(this->getModuleName(), "OpenGL renderer initialized successfully");
		}
		else {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to initialize OpenGL renderer");
			return ErrorCode::FailedInitRenderer;
		}

		this->loadCallbacks();
		this->_isRunning = true;

		while (_isRunning) {
			this->onUpdate();
			this->onRender();
			this->_pWindow->onUpdate();
		}

		return ErrorCode::None;
	}

	void Application::onUpdate()
	{
		
	}

	void Application::onRender()
	{
		this->_pRenderer->RenderFrame(this->_pWindow->getTime());
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
				this->getRenderer()->SetViewport(0, 0, resizeEvent.getWidth(), resizeEvent.getHeight());
			}
			});
	}

	Application::~Application()
	{
		TripleLogger::TLogger::Info("Application stoping...");
	}
}