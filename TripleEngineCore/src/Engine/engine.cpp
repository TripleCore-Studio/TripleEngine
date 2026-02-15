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
		std::unordered_map<std::type_index, void*> servicesFast;
		std::unordered_map<std::type_index, void*> systemsFast;

		std::unordered_map<std::type_index, std::unique_ptr<IService>> services;
		std::unordered_map<std::type_index, std::unique_ptr<ISystem>> systems;

		System::RenderSystem* pRenderSystem = nullptr;
		System::InputSystem* pInputSystem = nullptr;
		System::InputActionSystem* pInputActionSystem = nullptr;

		Service::EventService* pEventService = nullptr;
		Service::AssetService* pAssetsService = nullptr;
		Service::ModuleService* pModuleService = nullptr;
		Service::ComponentService* pComponentService = nullptr;

		std::unique_ptr<IWindow> pWindow;
		std::unique_ptr<Scene::Scene> pScene;
		Scene::Entity cameraEntity = Scene::INVALID_ENTITY;
	};

	template<typename T>
	T* registerService(std::unique_ptr<T> ptr, TripleEngineCore::Engine::Impl* _impl)
	{
		static_assert(std::is_base_of_v<IService, T>, "T must derive from IService");

		T* raw = ptr.get();

		std::unique_ptr<IService> basePtr = std::move(ptr);

		_impl->services.emplace(std::type_index(typeid(T)), std::move(basePtr));
		_impl->servicesFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	template<typename T>
	T* registerSystem(std::unique_ptr<T> ptr, TripleEngineCore::Engine::Impl* _impl)
	{
		static_assert(std::is_base_of_v<ISystem, T>, "T must derive from ISystem");

		T* raw = ptr.get();

		std::unique_ptr<ISystem> basePtr = std::move(ptr);

		_impl->systems.emplace(std::type_index(typeid(T)), std::move(basePtr));
		_impl->systemsFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	Engine::Engine() : _impl(new Impl())
	{
		TripleLogger::TLogger::Info("Engine starting...");

		this->_isRunning = false;
		this->_initialized = false;
		this->_lastTime = 0.0f;
		_impl->cameraEntity = Scene::INVALID_ENTITY;
	}

	bool Engine::init()
	{
		if (_initialized) {
			TripleLogger::TLogger::ModuleWarn(this->getModuleName(), "Engine already initialized");
			return true;
		}

		_impl->pEventService = registerService(std::make_unique<Service::EventService>(), _impl);
		_impl->pComponentService = registerService(std::make_unique<Service::ComponentService>(), _impl);
		_impl->pModuleService = registerService(std::make_unique<Service::ModuleService>("modules"), _impl);
		_impl->pAssetsService = registerService(std::make_unique<Service::AssetService>(), _impl);

		_impl->pRenderSystem = registerSystem(std::make_unique<System::RenderSystem>(_impl->pAssetsService), _impl);
		_impl->pInputSystem = registerSystem(std::make_unique<System::InputSystem>(), _impl);
		_impl->pInputActionSystem = registerSystem(std::make_unique<System::InputActionSystem>(_impl->pInputSystem), _impl);

		_impl->pModuleService->loadModule(ModuleType::OpenGLRenderer);
		_impl->pInputSystem->init();
		_impl->pScene = std::make_unique<Scene::Scene>(_impl->pComponentService);

		loadSystemCallbacks();
		loadAssetsCallbacks();

		if (!bootstrapComponents()) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error: bootstrapComponents");
			return false;
		}

		_initialized = true;
		return _initialized;
	}

	Engine::ErrorCode Engine::run(const char* title, unsigned int width, unsigned int height)
	{
		if(!this->_initialized) {
			TripleLogger::TLogger::Critical(this->getModuleName(), "Engine not initialized. Call init() before run().");
			return ErrorCode::FailedInitEngine;
		}

		OpenGLRenderModule* openGLModule = dynamic_cast<OpenGLRenderModule*>(_impl->pModuleService->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		IRenderer* renderer = openGLModule->getRenderer();
		if (renderer == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}
		_impl->pRenderSystem->setRenderer(renderer);

		IOpenGLRenderer* pGLRenderer = dynamic_cast<IOpenGLRenderer*>(renderer);
		if (!pGLRenderer) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		auto window = std::make_unique<GLWindow>(title, width, height, _impl->pEventService);
		void* loader = nullptr;
		if (window->init(&loader) != GLWindow::ErrorCode::None) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}
		_impl->pWindow = std::move(window);

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
		_impl->pRenderSystem->setRenderer(pGLRenderer);

		if (!bootstrapResources()) {
			_impl->pWindow->shutdown();
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error: bootstrapResources");
			return ErrorCode::FailedBootstrapResources;
		}

		_impl->pEventService->dispatch(Event::EngineLoadedEvent());

		_lastTime = _impl->pWindow->getTime();
		this->_isRunning = true;
		while (_isRunning) {
			float currentTime = _impl->pWindow->getTime();
			float dt = currentTime - _lastTime;
			_lastTime = currentTime;

			_impl->pWindow->PollEvents();
			_impl->pWindow->SwapBuffers();

			this->onUpdate(dt);

			_impl->pInputActionSystem->update(dt);
			_impl->pInputSystem->update(dt);

			this->onRender(_lastTime);
		}

		return ErrorCode::None;
	}

	void Engine::onUpdate(float dt)
	{
	}

	void Engine::onRender(float t)
	{
		Scene::Scene* scene = _impl->pScene.get();
		auto* transform = scene->getComponent<Scene::TransformComponent>(_impl->cameraEntity);
		auto* cameraComp = scene->getComponent<Scene::CameraComponent>(_impl->cameraEntity);

		if (!transform || !cameraComp) {
			TripleLogger::TLogger::ModuleWarn("Engine", "Active camera missing on render");
			return;
		}

		Graphics::FrameContext ctx;

		ctx.camera = Graphics::CameraData{ Utils::getViewMatrix(*transform),
			Utils::getProjectionMatrix(*cameraComp), transform->position};

		_impl->pRenderSystem->buildRenderCommands(_impl->pScene.get(), ctx.commands);
		ctx.time = t;

		_impl->pRenderSystem->getRenderer()->BeginFrame(t);
		_impl->pRenderSystem->getRenderer()->RenderFrame(ctx);
		_impl->pRenderSystem->getRenderer()->EndFrame();
	}

	Scene::Scene* Engine::getActiveScene()
	{
		return _impl->pScene.get();
	}

	IWindow* Engine::getWindow()
	{
		return _impl->pWindow.get();
	}

	void Engine::setActiveCamera(Scene::Entity camera)
	{
		_impl->cameraEntity = camera;
	}

	bool Engine::bootstrapComponents()
	{
		std::vector<Scene::ComponentTypeID> ids;
		ids.push_back(_impl->pComponentService->registerComponent<Scene::TransformComponent>());
		ids.push_back(_impl->pComponentService->registerComponent<Scene::MeshComponent>());
		ids.push_back(_impl->pComponentService->registerComponent<Scene::CameraComponent>());
		ids.push_back(_impl->pComponentService->registerComponent<Scene::ParentComponent>());
		ids.push_back(_impl->pComponentService->registerComponent<Scene::ChildrenComponent>());
		ids.push_back(_impl->pComponentService->registerComponent<Scene::NameComponent>());

		for (auto& id : ids) {
			if (id == Scene::INVALID_COMPONENT_TYPE_ID) {
				return false;
			}
		}

		return true;
	}

	void Engine::loadSystemCallbacks()
	{
		_impl->pEventService->addListener<Event::WindowCloseEvent>([this](Event::WindowCloseEvent& e) {
			TripleLogger::TLogger::Warn("Window ({}) closed", e.getTitle());
			_impl->pWindow->shutdown();
			this->_isRunning = false;
			});

		_impl->pEventService->addListener<Event::WindowResizeEvent>([this](Event::WindowResizeEvent& e) {
			_impl->pRenderSystem->getRenderer()->SetViewport(0, 0, e.getWidth(), e.getHeight());
			});

		_impl->pEventService->addListener<Event::KeyboardInputEvent>([this](Event::KeyboardInputEvent& e) {
			_impl->pInputSystem->onKeyboard(e);
			});

		_impl->pEventService->addListener<Event::MouseMoveEvent>([this](Event::MouseMoveEvent& e) {
			_impl->pInputSystem->onMouseMove(e);
			});

		_impl->pEventService->addListener<Event::MouseButtonEvent>([this](Event::MouseButtonEvent& e) {
			_impl->pInputSystem->onMouseButton(e);
		});
	}

	void Engine::loadAssetsCallbacks()
	{
		_impl->pAssetsService->setTextureLoadedCallback([this](const Asset::Texture* tex) {
			_impl->pRenderSystem->uploadTexture(tex);
		});

		_impl->pAssetsService->setModelLoadedCallback([this](const Asset::Model* model) {
			_impl->pRenderSystem->uploadGeometry(model);
		});

		_impl->pAssetsService->setShaderLoadedCallback([this](const Asset::Shader* shader) {
			_impl->pRenderSystem->uploadShader(shader);
		});
	}

	void* Engine::getServiceRaw(const std::type_info& type)
	{
		auto it = _impl->servicesFast.find(std::type_index(type));
		if (it != _impl->servicesFast.end()) {
			return it->second;
		}
		return nullptr;
	}

	void* Engine::getSystemRaw(const std::type_info& type)
	{
		auto it = _impl->systemsFast.find(std::type_index(type));
		if (it != _impl->systemsFast.end()) {
			return it->second;
		}
		return nullptr;
	}

	bool Engine::bootstrapResources()
	{
		return _impl->pAssetsService->loadDefaultAssets();
	}

	Engine::~Engine()
	{
		TripleLogger::TLogger::Info("Engine stoping...");
		delete _impl;
	}
}