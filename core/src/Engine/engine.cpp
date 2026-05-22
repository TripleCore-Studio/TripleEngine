#include "triple/core/Engine/Engine.h"

#include <thread>
#include <chrono>
#include <unordered_map>

#include "triple/core/CoreTypes.h"

#include "triple/core/Modules/OpenGLModule.h"
#include <triple/gfx/IOpenGLRenderer.h>
#include <triple/log/Logger.h>
#include "triple/core/Utils/CameraUtils.h"
#include "triple/core/Utils/TransformUtils.h"
#include "triple/core/Engine/GLWindow.h"

#include "triple/core/Scene/Scene.h"
#include "triple/core/Scene/MeshComponent.h"
#include "triple/core/Scene/ParentComponent.h"
#include "triple/core/Scene/ChildrenComponent.h"
#include "triple/core/Scene/NameComponent.h"
#include "triple/core/Scene/SceneTypes.h"

#include "triple/core/Event/EngineLoadedEvent.h"
#include "triple/core/Event/WindowCloseEvent.h"
#include "triple/core/Event/WindowResizeEvent.h"
#include "triple/core/Event/KeyboardInputEvent.h"
#include "triple/core/Event/MouseMoveEvent.h"
#include "triple/core/Event/MouseButtonEvent.h"

#include "triple/core/Service/ComponentService.h"
#include "triple/core/Service/EventService.h"
#include "triple/core/Service/AssetService.h"
#include "triple/core/Service/ModuleService.h"

#include "triple/core/System/RenderSystem.h"
#include "triple/core/System/InputSystem.h"
#include "triple/core/System/InputActionSystem.h"

namespace triple::core {
	struct Engine::Impl {
		std::unordered_map<std::type_index, void*> m_servicesFast;
		std::unordered_map<std::type_index, void*> m_systemsFast;

		std::unordered_map<std::type_index, std::unique_ptr<IService>> m_services;
		std::unordered_map<std::type_index, std::unique_ptr<ISystem>> m_systems;

		RenderSystem* m_renderSystem = nullptr;
		InputSystem* m_inputSystem = nullptr;
		InputActionSystem* m_inputActionSystem = nullptr;

		EventService* m_eventService = nullptr;
		AssetService* m_assetsService = nullptr;
		ModuleService* m_moduleService = nullptr;
		ComponentService* m_componentService = nullptr;

		std::unique_ptr<IWindow> m_window;
		std::unique_ptr<Scene> m_scene;
		Entity m_cameraEntity = INVALID_ENTITY;
	};

	template<typename T>
	T* registerService(std::unique_ptr<T> ptr, Engine::Impl* _impl)
	{
		static_assert(std::is_base_of_v<IService, T>, "T must derive from IService");

		T* raw = ptr.get();

		std::unique_ptr<IService> basePtr = std::move(ptr);

		_impl->m_services.emplace(std::type_index(typeid(T)), std::move(basePtr));
		_impl->m_servicesFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	template<typename T>
	T* registerSystem(std::unique_ptr<T> ptr,	Engine::Impl* _impl)
	{
		static_assert(std::is_base_of_v<ISystem, T>, "T must derive from ISystem");

		T* raw = ptr.get();

		std::unique_ptr<ISystem> basePtr = std::move(ptr);

		_impl->m_systems.emplace(std::type_index(typeid(T)), std::move(basePtr));
		_impl->m_systemsFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	Engine::Engine() : m_impl(new Impl())
	{
		triple::log::Logger::Info("Engine starting...");

		this->m_isRunning = false;
		this->m_isInitialized = false;
		this->m_lastTime = 0.0f;
		m_impl->m_cameraEntity = INVALID_ENTITY;
	}

	bool Engine::init()
	{
		if (m_isInitialized) {
			triple::log::Logger::ModuleWarn(this->getModuleName(), "Engine already initialized");
			return true;
		}

		m_impl->m_eventService = registerService(std::make_unique<EventService>(), m_impl);
		m_impl->m_componentService = registerService(std::make_unique<ComponentService>(), m_impl);
		m_impl->m_moduleService = registerService(std::make_unique<ModuleService>("modules"), m_impl);
		m_impl->m_assetsService = registerService(std::make_unique<AssetService>(), m_impl);

		m_impl->m_renderSystem = registerSystem(std::make_unique<RenderSystem>(m_impl->m_assetsService), m_impl);
		m_impl->m_inputSystem = registerSystem(std::make_unique<InputSystem>(), m_impl);
		m_impl->m_inputActionSystem = registerSystem(std::make_unique<InputActionSystem>(m_impl->m_inputSystem), m_impl);

		m_impl->m_moduleService->loadModule(ModuleType::OpenGLRenderer);
		m_impl->m_inputSystem->init();
		m_impl->m_scene = std::make_unique<Scene>(m_impl->m_componentService);
		loadSystemCallbacks();
		loadAssetsCallbacks();

		if (!bootstrapComponents()) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Error: bootstrapComponents");
			return false;
		}

		m_isInitialized = true;
		return m_isInitialized;
	}

	Engine::ErrorCode Engine::run(const char* title, unsigned int width, unsigned int height)
	{
		if(!this->m_isInitialized) {
			triple::log::Logger::Critical(this->getModuleName(), "Engine not initialized. Call init() before run().");
			return ErrorCode::FailedInitEngine;
		}

		OpenGLRenderModule* openGLModule = dynamic_cast<OpenGLRenderModule*>(m_impl->m_moduleService->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		gfx::IRenderer* renderer = openGLModule->getRenderer();
		if (renderer == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}
		m_impl->m_renderSystem->setRenderer(renderer);

		gfx::IOpenGLRenderer* pGLRenderer = dynamic_cast<gfx::IOpenGLRenderer*>(renderer);
		if (!pGLRenderer) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		auto window = std::make_unique<GLWindow>(title, width, height, m_impl->m_eventService);
		void* loader = nullptr;
		if (window->init(&loader) != GLWindow::ErrorCode::None) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}
		m_impl->m_window = std::move(window);

		if (loader == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Loader for OpenGL not initialized");
			return ErrorCode::FailedToLoadWindow;
		}

		if (pGLRenderer->initGlad(loader)) {
			triple::log::Logger::ModuleInfo(this->getModuleName(), "OpenGL renderer initialized successfully");
		}
		else {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to initialize OpenGL renderer");
			return ErrorCode::FailedInitRenderer;
		}

		pGLRenderer->Initialize();
		m_impl->m_renderSystem->setRenderer(pGLRenderer);

		if (!bootstrapResources()) {
			m_impl->m_window->shutdown();
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Error: bootstrapResources");
			return ErrorCode::FailedBootstrapResources;
		}

		auto event = EngineLoadedEvent();
		m_impl->m_eventService->dispatch(event);

		m_lastTime = m_impl->m_window->getTime();
		this->m_isRunning = true;
		while (m_isRunning) {
			float currentTime = m_impl->m_window->getTime();
			float dt = currentTime - m_lastTime;
			m_lastTime = currentTime;

			m_impl->m_window->PollEvents();
			m_impl->m_window->SwapBuffers();

			this->onUpdate(dt);

			m_impl->m_inputActionSystem->update(dt);
			m_impl->m_inputSystem->update(dt);

			this->onRender(m_lastTime);
		}

		return ErrorCode::None;
	}

	void Engine::onUpdate(float dt)
	{
	}

	void Engine::onRender(float t)
	{
		Scene* scene = m_impl->m_scene.get();
		auto* transform = scene->getComponent<TransformComponent>(m_impl->m_cameraEntity);
		auto* cameraComp = scene->getComponent<CameraComponent>(m_impl->m_cameraEntity);

		if (!transform || !cameraComp) {
			triple::log::Logger::ModuleWarn(this->getModuleName(), "Active camera missing on render");
			return;
		}

		gfx::FrameContext ctx;

		ctx.camera = gfx::CameraData{ getViewMatrix(*transform),
			getProjectionMatrix(*cameraComp), transform->position};

		m_impl->m_renderSystem->buildRenderCommands(m_impl->m_scene.get(), ctx.commands);
		ctx.time = t;

		m_impl->m_renderSystem->getRenderer()->BeginFrame(t);
		m_impl->m_renderSystem->getRenderer()->RenderFrame(ctx);
		m_impl->m_renderSystem->getRenderer()->EndFrame();
	}

	Scene* Engine::getActiveScene()
	{
		return m_impl->m_scene.get();
	}

	IWindow* Engine::getWindow()
	{
		return m_impl->m_window.get();
	}

	void Engine::setActiveCamera(Entity camera)
	{
		m_impl->m_cameraEntity = camera;
	}

	bool Engine::bootstrapComponents()
	{
		std::vector<ComponentTypeID> ids;
		ids.push_back(m_impl->m_componentService->registerComponent<TransformComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<MeshComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<CameraComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<ParentComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<ChildrenComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<NameComponent>());

		for (auto& id : ids) {
			if (id == INVALID_COMPONENT_TYPE_ID) {
				return false;
			}
		}

		return true;
	}
	
	void Engine::loadSystemCallbacks()
	{
		m_impl->m_eventService->addListener<WindowCloseEvent>([this](WindowCloseEvent& e) {
			triple::log::Logger::Warn("Window ({}) closed", e.getTitle());
			m_impl->m_window->shutdown();
			this->m_isRunning = false;
			});

		m_impl->m_eventService->addListener<WindowResizeEvent>([this](WindowResizeEvent& e) {
			m_impl->m_renderSystem->getRenderer()->SetViewport(0, 0, e.getWidth(), e.getHeight());
			});

		m_impl->m_eventService->addListener<KeyboardInputEvent>([this](KeyboardInputEvent& e) {
			m_impl->m_inputSystem->onKeyboard(e);
			});

		m_impl->m_eventService->addListener<MouseMoveEvent>([this](MouseMoveEvent& e) {
			m_impl->m_inputSystem->onMouseMove(e);
			});

		m_impl->m_eventService->addListener<MouseButtonEvent>([this](MouseButtonEvent& e) {
			m_impl->m_inputSystem->onMouseButton(e);
		});
	}

	void Engine::loadAssetsCallbacks()
	{
		m_impl->m_assetsService->setTextureLoadedCallback([this](const Texture* tex) {
			m_impl->m_renderSystem->uploadTexture(tex);
		});

		m_impl->m_assetsService->setModelLoadedCallback([this](const Model* model) {
			m_impl->m_renderSystem->uploadGeometry(model);
		});

		m_impl->m_assetsService->setShaderLoadedCallback([this](const Shader* shader) {
			m_impl->m_renderSystem->uploadShader(shader);
		});
	}

	void* Engine::getServiceRaw(const std::type_info& type)
	{
		auto it = m_impl->m_servicesFast.find(std::type_index(type));
		if (it != m_impl->m_servicesFast.end()) {
			return it->second;
		}
		return nullptr;
	}

	void* Engine::getSystemRaw(const std::type_info& type)
	{
		auto it = m_impl->m_systemsFast.find(std::type_index(type));
		if (it != m_impl->m_systemsFast.end()) {
			return it->second;
		}
		return nullptr;
	}

	bool Engine::bootstrapResources()
	{
		return m_impl->m_assetsService->loadDefaultAssets();
	}

	Engine::~Engine()
	{
		triple::log::Logger::Info("Engine stoping...");
		delete m_impl;
	}
}