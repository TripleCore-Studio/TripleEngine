#include "triple/core/base/Engine.h"

#include <triple/gfx/IOpenGLRenderer.h>
#include <triple/log/Logger.h>

#include "triple/core/platform/OpenGLModule.h"
#include "triple/core/platform/OpenGLModule.h"

#include "triple/core/platform/GLWindow.h"
#include "triple/core/platform/ModuleService.h"

#include "triple/core/event/WindowCloseEvent.h"
#include "triple/core/event/WindowResizeEvent.h"
#include "triple/core/event/KeyboardInputEvent.h"
#include "triple/core/event/MouseMoveEvent.h"
#include "triple/core/event/MouseButtonEvent.h"

#include "triple/core/input/InputSystem.h"
#include "triple/core/event/EventBus.h"

namespace triple::core {
	struct Engine::Impl {
		std::unique_ptr<InputSystem> inputSystem;
		std::unique_ptr<EventBus> bus;
		std::unique_ptr<ModuleService> moduleService;
		std::unique_ptr<IWindow> window;
		gfx::IRenderer *renderer = nullptr;
	};

	Engine::Engine()
	    : m_impl(new Impl()), m_isRunning(false), m_isInitialized(false), m_lastTime(0.0f) {
		triple::log::Logger::Info("Engine starting...");
	}

	bool Engine::init() {
		if (m_isInitialized) {
			triple::log::Logger::ModuleWarn(this->getModuleName(), "Engine already initialized");
			return true;
		}

		m_impl->bus = std::make_unique<EventBus>();
		m_impl->moduleService = std::make_unique<ModuleService>("modules");
		m_impl->inputSystem = std::make_unique<InputSystem>();

		m_impl->moduleService->loadModule(ModuleType::OpenGLRenderer);
		m_impl->inputSystem->init();

		loadSystemCallbacks();

		m_isInitialized = true;
		return m_isInitialized;
	}

	Engine::ErrorCode Engine::run(const char *title, unsigned int width, unsigned int height) {
		if (!this->m_isInitialized) {
			triple::log::Logger::Critical(this->getModuleName(),
			                              "Engine not initialized. Call init() before run().");
			return ErrorCode::FailedInitEngine;
		}

		OpenGLRenderModule *openGLModule = dynamic_cast<OpenGLRenderModule *>(
		    m_impl->moduleService->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(),
			                                    "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		gfx::IRenderer *renderer = openGLModule->getRenderer();
		if (renderer == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(),
			                                    "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}
		m_impl->renderer = renderer;

		gfx::IOpenGLRenderer *pGLRenderer = dynamic_cast<gfx::IOpenGLRenderer *>(renderer);
		if (!pGLRenderer) {
			triple::log::Logger::ModuleCritical(
			    this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		auto window = std::make_unique<GLWindow>(title, width, height, m_impl->bus.get());
		void *loader = nullptr;
		if (window->init(&loader) != GLWindow::ErrorCode::None) {
			triple::log::Logger::ModuleCritical(this->getModuleName(),
			                                    "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}
		m_impl->window = std::move(window);

		if (loader == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(),
			                                    "Loader for OpenGL not initialized");
			return ErrorCode::FailedToLoadWindow;
		}

		if (pGLRenderer->initGlad(loader)) {
			triple::log::Logger::ModuleInfo(this->getModuleName(),
			                                "OpenGL renderer initialized successfully");
		} else {
			triple::log::Logger::ModuleCritical(this->getModuleName(),
			                                    "Failed to initialize OpenGL renderer");
			return ErrorCode::FailedInitRenderer;
		}

		pGLRenderer->Initialize();

		EngineContext ctx;
		ctx.renderer = pGLRenderer;
		ctx.bus = m_impl->bus.get();
		ctx.window = m_impl->window.get();
		ctx.inputSystem = m_impl->inputSystem.get();

		for (auto &layer : m_layerStack) {
			layer->onAttach(ctx);
			layer->isAttached = true;
		}

		m_lastTime = m_impl->window->getTime();
		this->m_isRunning = true;
		while (m_isRunning) {
			float currentTime = m_impl->window->getTime();
			float dt = currentTime - m_lastTime;
			m_lastTime = currentTime;

			m_impl->window->pollEvents();

			for (auto &layer : m_layerStack)
				layer->onUpdate(dt);

			m_impl->inputSystem->update(dt);

			m_impl->renderer->BeginFrame(m_lastTime);

			for (auto &layer : m_layerStack)
				layer->onRender(m_lastTime);

			m_impl->renderer->EndFrame();

			m_impl->window->swapBuffers();
		}

		return ErrorCode::None;
	}

	IWindow *Engine::getWindow() const { return m_impl->window.get(); }

	void Engine::loadSystemCallbacks() {
		m_impl->bus->addListener([this](Event &e) { dispatchToLayers(e); });

		m_impl->bus->addListener<WindowCloseEvent>([this](WindowCloseEvent &e) {
			triple::log::Logger::Warn("Window ({}) closed", e.getTitle());
			m_impl->window->shutdown();
			this->m_isRunning = false;
		});

		m_impl->bus->addListener<WindowResizeEvent>([this](WindowResizeEvent &e) {
			m_impl->renderer->SetViewport(0, 0, e.getWidth(), e.getHeight());
		});

		m_impl->bus->addListener<KeyboardInputEvent>(
		    [this](KeyboardInputEvent &e) { m_impl->inputSystem->onKeyboard(e); });

		m_impl->bus->addListener<MouseMoveEvent>(
		    [this](MouseMoveEvent &e) { m_impl->inputSystem->onMouseMove(e); });

		m_impl->bus->addListener<MouseButtonEvent>(
		    [this](MouseButtonEvent &e) { m_impl->inputSystem->onMouseButton(e); });
	}

	void Engine::dispatchToLayers(Event &e) {
		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it) {
			if (e.handled)
				break;
			if (!(*it)->isAttached)
				continue;
			(*it)->onEvent(e);
		}
	}

	void Engine::pushLayer(std::unique_ptr<Layer> layer) {
		m_layerStack.pushLayer(std::move(layer));
	}
	void Engine::pushOverlay(std::unique_ptr<Layer> overlay) {
		m_layerStack.pushOverlay(std::move(overlay));
	}

	Engine::~Engine() {
		triple::log::Logger::Info("Engine stoping...");
		delete m_impl;
	}
} // namespace triple::core