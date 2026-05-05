#include "Engine/Engine.h"

#include <thread>
#include <chrono>
#include <unordered_map>

#include "Core/CoreTypes.h"

#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "TLogger.h"
#include "Utils/CameraUtils.h"
#include "Utils/TransformUtils.h"
#include "Engine/GLWindow.h"

#include "Scene/Scene.h"
#include "Scene/MeshComponent.h"
#include "Scene/ParentComponent.h"
#include "Scene/ChildrenComponent.h"
#include "Scene/NameComponent.h"
#include "Scene/SceneTypes.h"

#include "Event/EngineLoadedEvent.h"
#include "Event/WindowCloseEvent.h"
#include "Event/WindowResizeEvent.h"
#include "Event/KeyboardInputEvent.h"
#include "Event/MouseMoveEvent.h"
#include "Event/MouseButtonEvent.h"

#include "Service/ComponentService.h"
#include "Service/EventService.h"
#include "Service/AssetService.h"
#include "Service/ModuleService.h"

#include "System/RenderSystem.h"
#include "System/InputSystem.h"
#include "System/InputActionSystem.h"

namespace TripleEngineCore {
	struct Engine::Impl {
		std::unordered_map<std::type_index, void*> m_servicesFast;
		std::unordered_map<std::type_index, void*> m_systemsFast;

		std::unordered_map<std::type_index, std::unique_ptr<IService>> m_services;
		std::unordered_map<std::type_index, std::unique_ptr<ISystem>> m_systems;

		System::RenderSystem* m_renderSystem = nullptr;
		System::InputSystem* m_inputSystem = nullptr;
		System::InputActionSystem* m_inputActionSystem = nullptr;

		Service::EventService* m_eventService = nullptr;
		Service::AssetService* m_assetsService = nullptr;
		Service::ModuleService* m_moduleService = nullptr;
		Service::ComponentService* m_componentService = nullptr;

		std::unique_ptr<IWindow> m_window;
		std::unique_ptr<Scene::Scene> m_scene;
		Scene::Entity m_cameraEntity = Scene::INVALID_ENTITY;
	};

	template<typename T>
	T* registerService(std::unique_ptr<T> ptr, TripleEngineCore::Engine::Impl* _impl)
	{
		static_assert(std::is_base_of_v<IService, T>, "T must derive from IService");

		T* raw = ptr.get();

		std::unique_ptr<IService> basePtr = std::move(ptr);

		_impl->m_services.emplace(std::type_index(typeid(T)), std::move(basePtr));
		_impl->m_servicesFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	template<typename T>
	T* registerSystem(std::unique_ptr<T> ptr, TripleEngineCore::Engine::Impl* _impl)
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
		TripleLogger::TLogger::Info("Engine starting...");

		this->m_isRunning = false;
		this->m_isInitialized = false;
		this->m_lastTime = 0.0f;
		m_impl->m_cameraEntity = Scene::INVALID_ENTITY;
	}

	bool Engine::init()
	{
		if (m_isInitialized) {
			TripleLogger::TLogger::ModuleWarn(this->getModuleName(), "Engine already initialized");
			return true;
		}

		m_impl->m_eventService = registerService(std::make_unique<Service::EventService>(), m_impl);
		m_impl->m_componentService = registerService(std::make_unique<Service::ComponentService>(), m_impl);
		m_impl->m_moduleService = registerService(std::make_unique<Service::ModuleService>("modules"), m_impl);
		m_impl->m_assetsService = registerService(std::make_unique<Service::AssetService>(), m_impl);

		m_impl->m_renderSystem = registerSystem(std::make_unique<System::RenderSystem>(m_impl->m_assetsService), m_impl);
		m_impl->m_inputSystem = registerSystem(std::make_unique<System::InputSystem>(), m_impl);
		m_impl->m_inputActionSystem = registerSystem(std::make_unique<System::InputActionSystem>(m_impl->m_inputSystem), m_impl);

		m_impl->m_moduleService->loadModule(ModuleType::OpenGLRenderer);
		m_impl->m_inputSystem->init();
		m_impl->m_scene = std::make_unique<Scene::Scene>(m_impl->m_componentService);

		loadSystemCallbacks();
		loadAssetsCallbacks();

		if (!bootstrapComponents()) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error: bootstrapComponents");
			return false;
		}

		m_isInitialized = true;
		return m_isInitialized;
	}

	Engine::ErrorCode Engine::run(const char* title, unsigned int width, unsigned int height)
	{
		if(!this->m_isInitialized) {
			TripleLogger::TLogger::Critical(this->getModuleName(), "Engine not initialized. Call init() before run().");
			return ErrorCode::FailedInitEngine;
		}

		OpenGLRenderModule* openGLModule = dynamic_cast<OpenGLRenderModule*>(m_impl->m_moduleService->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		IRenderer* renderer = openGLModule->getRenderer();
		if (renderer == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}
		m_impl->m_renderSystem->setRenderer(renderer);

		IOpenGLRenderer* pGLRenderer = dynamic_cast<IOpenGLRenderer*>(renderer);
		if (!pGLRenderer) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		auto window = std::make_unique<GLWindow>(title, width, height, m_impl->m_eventService);
		void* loader = nullptr;
		if (window->init(&loader) != GLWindow::ErrorCode::None) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}
		m_impl->m_window = std::move(window);

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

		pGLRenderer->Initialize();
		m_impl->m_renderSystem->setRenderer(pGLRenderer);

		if (!bootstrapResources()) {
			m_impl->m_window->shutdown();
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error: bootstrapResources");
			return ErrorCode::FailedBootstrapResources;
		}

		auto event = Event::EngineLoadedEvent();
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
		Scene::Scene* scene = m_impl->m_scene.get();
		auto* transform = scene->getComponent<Scene::TransformComponent>(m_impl->m_cameraEntity);
		auto* cameraComp = scene->getComponent<Scene::CameraComponent>(m_impl->m_cameraEntity);

		if (!transform || !cameraComp) {
			TripleLogger::TLogger::ModuleWarn("Engine", "Active camera missing on render");
			return;
		}

		Graphics::FrameContext ctx;

		ctx.camera = Graphics::CameraData{ Utils::getViewMatrix(*transform),
			Utils::getProjectionMatrix(*cameraComp), transform->position};

		m_impl->m_renderSystem->buildRenderCommands(m_impl->m_scene.get(), ctx.commands);
		ctx.time = t;

		m_impl->m_renderSystem->getRenderer()->BeginFrame(t);
		m_impl->m_renderSystem->getRenderer()->RenderFrame(ctx);
		m_impl->m_renderSystem->getRenderer()->EndFrame();
	}

	Scene::Scene* Engine::getActiveScene()
	{
		return m_impl->m_scene.get();
	}

	IWindow* Engine::getWindow()
	{
		return m_impl->m_window.get();
	}

	void Engine::setActiveCamera(Scene::Entity camera)
	{
		m_impl->m_cameraEntity = camera;
	}

	bool Engine::bootstrapComponents()
	{
		std::vector<Scene::ComponentTypeID> ids;
		ids.push_back(m_impl->m_componentService->registerComponent<Scene::TransformComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<Scene::MeshComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<Scene::CameraComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<Scene::ParentComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<Scene::ChildrenComponent>());
		ids.push_back(m_impl->m_componentService->registerComponent<Scene::NameComponent>());

		for (auto& id : ids) {
			if (id == Scene::INVALID_COMPONENT_TYPE_ID) {
				return false;
			}
		}

		return true;
	}
	
	void Engine::loadSystemCallbacks()
	{
		m_impl->m_eventService->addListener<Event::WindowCloseEvent>([this](Event::WindowCloseEvent& e) {
			TripleLogger::TLogger::Warn("Window ({}) closed", e.getTitle());
			m_impl->m_window->shutdown();
			this->m_isRunning = false;
			});

		m_impl->m_eventService->addListener<Event::WindowResizeEvent>([this](Event::WindowResizeEvent& e) {
			m_impl->m_renderSystem->getRenderer()->SetViewport(0, 0, e.getWidth(), e.getHeight());
			});

		m_impl->m_eventService->addListener<Event::KeyboardInputEvent>([this](Event::KeyboardInputEvent& e) {
			m_impl->m_inputSystem->onKeyboard(e);
			});

		m_impl->m_eventService->addListener<Event::MouseMoveEvent>([this](Event::MouseMoveEvent& e) {
			m_impl->m_inputSystem->onMouseMove(e);
			});

		m_impl->m_eventService->addListener<Event::MouseButtonEvent>([this](Event::MouseButtonEvent& e) {
			m_impl->m_inputSystem->onMouseButton(e);
		});
	}

	void Engine::loadAssetsCallbacks()
	{
		m_impl->m_assetsService->setTextureLoadedCallback([this](const Asset::Texture* tex) {
			m_impl->m_renderSystem->uploadTexture(tex);
		});

		m_impl->m_assetsService->setModelLoadedCallback([this](const Asset::Model* model) {
			m_impl->m_renderSystem->uploadGeometry(model);
		});

		m_impl->m_assetsService->setShaderLoadedCallback([this](const Asset::Shader* shader) {
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
		TripleLogger::TLogger::Info("Engine stoping...");
		delete m_impl;
	}
}