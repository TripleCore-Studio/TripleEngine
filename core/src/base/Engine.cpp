#include "triple/core/base/Engine.h"

#include <unordered_map>

#include <triple/gfx/IOpenGLRenderer.h>
#include <triple/log/Logger.h>

#include "triple/core/platform/OpenGLModule.h"
#include "triple/core/platform/OpenGLModule.h"
#include "triple/core/utils/CameraUtils.h"

#include "triple/core/platform/GLWindow.h"
#include "triple/core/platform/ModuleService.h"

#include "triple/core/ecs/Scene.h"
#include "triple/core/ecs/MeshComponent.h"
#include "triple/core/ecs/ParentComponent.h"
#include "triple/core/ecs/ChildrenComponent.h"
#include "triple/core/ecs/NameComponent.h"
#include "triple/core/ecs/SceneTypes.h"

#include "triple/core/event/EngineLoadedEvent.h"
#include "triple/core/event/WindowCloseEvent.h"
#include "triple/core/event/WindowResizeEvent.h"
#include "triple/core/event/KeyboardInputEvent.h"
#include "triple/core/event/MouseMoveEvent.h"
#include "triple/core/event/MouseButtonEvent.h"

#include "triple/core/input/InputSystem.h"
#include "triple/core/input/InputActionSystem.h"

#include "triple/core/ecs/ComponentService.h"
#include "triple/core/event/EventService.h"
#include "triple/core/asset/AssetService.h"


#include "triple/core/render/RenderSystem.h"

namespace triple::core {
	struct Engine::Impl {
		std::unordered_map<std::type_index, void*> servicesFast;
		std::unordered_map<std::type_index, void*> systemsFast;

		std::unordered_map<std::type_index, std::unique_ptr<IService>> services;
		std::unordered_map<std::type_index, std::unique_ptr<ISystem>> systems;

		RenderSystem* renderSystem = nullptr;
		InputSystem* inputSystem = nullptr;
		InputActionSystem* inputActionSystem = nullptr;

		EventService* eventService = nullptr;
		AssetService* assetsService = nullptr;
		ModuleService* moduleService = nullptr;
		ComponentService* componentService = nullptr;

		std::unique_ptr<IWindow> window;
		std::unique_ptr<Scene> scene;
		Entity cameraEntity = INVALID_ENTITY;
	};

	template<typename T>
	T* registerService(std::unique_ptr<T> ptr, Engine::Impl* impl)
	{
		static_assert(std::is_base_of_v<IService, T>, "T must derive from IService");

		T* raw = ptr.get();

		std::unique_ptr<IService> basePtr = std::move(ptr);

		impl->services.emplace(std::type_index(typeid(T)), std::move(basePtr));
		impl->servicesFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	template<typename T>
	T* registerSystem(std::unique_ptr<T> ptr,	Engine::Impl* impl)
	{
		static_assert(std::is_base_of_v<ISystem, T>, "T must derive from ISystem");

		T* raw = ptr.get();

		std::unique_ptr<ISystem> basePtr = std::move(ptr);

		impl->systems.emplace(std::type_index(typeid(T)), std::move(basePtr));
		impl->systemsFast[std::type_index(typeid(T))] = raw;

		return raw;
	}

	Engine::Engine() : impl(new Impl()), m_isRunning(false), m_isInitialized(false), m_lastTime(0.0f)
	{
		triple::log::Logger::Info("Engine starting...");
		impl->cameraEntity = INVALID_ENTITY;
	}
	
	bool Engine::init()
	{
		if (m_isInitialized) {
			triple::log::Logger::ModuleWarn(this->getModuleName(), "Engine already initialized");
			return true;
		}

		impl->eventService = registerService(std::make_unique<EventService>(), impl);
		impl->componentService = registerService(std::make_unique<ComponentService>(), impl);
		impl->moduleService = registerService(std::make_unique<ModuleService>("modules"), impl);
		impl->assetsService = registerService(std::make_unique<AssetService>(), impl);

		impl->renderSystem = registerSystem(std::make_unique<RenderSystem>(impl->assetsService), impl);
		impl->inputSystem = registerSystem(std::make_unique<InputSystem>(), impl);
		impl->inputActionSystem = registerSystem(std::make_unique<InputActionSystem>(impl->inputSystem), impl);

		impl->moduleService->loadModule(ModuleType::OpenGLRenderer);
		impl->inputSystem->init();
		impl->scene = std::make_unique<Scene>(impl->componentService);
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

		OpenGLRenderModule* openGLModule = dynamic_cast<OpenGLRenderModule*>(impl->moduleService->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		gfx::IRenderer* renderer = openGLModule->getRenderer();
		if (renderer == nullptr) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}
		impl->renderSystem->setRenderer(renderer);

		gfx::IOpenGLRenderer* pGLRenderer = dynamic_cast<gfx::IOpenGLRenderer*>(renderer);
		if (!pGLRenderer) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		auto window = std::make_unique<GLWindow>(title, width, height, impl->eventService);
		void* loader = nullptr;
		if (window->init(&loader) != GLWindow::ErrorCode::None) {
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}
		impl->window = std::move(window);

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
		impl->renderSystem->setRenderer(pGLRenderer);

		if (!bootstrapResources()) {
			impl->window->shutdown();
			triple::log::Logger::ModuleCritical(this->getModuleName(), "Error: bootstrapResources");
			return ErrorCode::FailedBootstrapResources;
		}

		auto event = EngineLoadedEvent();
		impl->eventService->dispatch(event);

		m_lastTime = impl->window->getTime();
		this->m_isRunning = true;
		while (m_isRunning) {
			float currentTime = impl->window->getTime();
			float dt = currentTime - m_lastTime;
			m_lastTime = currentTime;

			impl->window->PollEvents();
			impl->window->SwapBuffers();

			this->onUpdate(dt);

			impl->inputActionSystem->update(dt);
			impl->inputSystem->update(dt);

			this->onRender(m_lastTime);
		}

		return ErrorCode::None;
	}

	void Engine::onUpdate(float dt)
	{
	}

	void Engine::onRender(float t)
	{
		Scene* scene = impl->scene.get();
		auto* transform = scene->getComponent<TransformComponent>(impl->cameraEntity);
		auto* cameraComp = scene->getComponent<CameraComponent>(impl->cameraEntity);

		if (!transform || !cameraComp) {
			triple::log::Logger::ModuleWarn(this->getModuleName(), "Active camera missing on render");
			return;
		}

		gfx::FrameContext ctx;

		ctx.camera = gfx::CameraData{ getViewMatrix(*transform),
			getProjectionMatrix(*cameraComp), transform->position};

		impl->renderSystem->buildRenderCommands(impl->scene.get(), ctx.commands);
		ctx.time = t;

		impl->renderSystem->getRenderer()->BeginFrame(t);
		impl->renderSystem->getRenderer()->RenderFrame(ctx);
		impl->renderSystem->getRenderer()->EndFrame();
	}

	Scene* Engine::getActiveScene() const
	{
		return impl->scene.get();
	}

	IWindow* Engine::getWindow() const
	{
		return impl->window.get();
	}

	void Engine::setActiveCamera(Entity camera) const
	{
		impl->cameraEntity = camera;
	}

	bool Engine::bootstrapComponents() const
	{
		std::vector<ComponentTypeID> ids;
		ids.push_back(impl->componentService->registerComponent<TransformComponent>());
		ids.push_back(impl->componentService->registerComponent<MeshComponent>());
		ids.push_back(impl->componentService->registerComponent<CameraComponent>());
		ids.push_back(impl->componentService->registerComponent<ParentComponent>());
		ids.push_back(impl->componentService->registerComponent<ChildrenComponent>());
		ids.push_back(impl->componentService->registerComponent<NameComponent>());

		for (auto& id : ids) {
			if (id == INVALID_COMPONENT_TYPE_ID) {
				return false;
			}
		}

		return true;
	}
	
	void Engine::loadSystemCallbacks()
	{
		impl->eventService->addListener<WindowCloseEvent>([this](WindowCloseEvent& e) {
			triple::log::Logger::Warn("Window ({}) closed", e.getTitle());
			impl->window->shutdown();
			this->m_isRunning = false;
			});

		impl->eventService->addListener<WindowResizeEvent>([this](WindowResizeEvent& e) {
			impl->renderSystem->getRenderer()->SetViewport(0, 0, e.getWidth(), e.getHeight());
			});

		impl->eventService->addListener<KeyboardInputEvent>([this](KeyboardInputEvent& e) {
			impl->inputSystem->onKeyboard(e);
			});

		impl->eventService->addListener<MouseMoveEvent>([this](MouseMoveEvent& e) {
			impl->inputSystem->onMouseMove(e);
			});

		impl->eventService->addListener<MouseButtonEvent>([this](MouseButtonEvent& e) {
			impl->inputSystem->onMouseButton(e);
		});
	}

	void Engine::loadAssetsCallbacks()
	{
		impl->assetsService->setTextureLoadedCallback([this](const Texture* tex) {
			impl->renderSystem->uploadTexture(tex);
		});

		impl->assetsService->setModelLoadedCallback([this](const Model* model) {
			impl->renderSystem->uploadGeometry(model);
		});

		impl->assetsService->setShaderLoadedCallback([this](const Shader* shader) {
			impl->renderSystem->uploadShader(shader);
		});
	}

	void* Engine::getServiceRaw(const std::type_info& type) const
	{
		auto it = impl->servicesFast.find(std::type_index(type));
		if (it != impl->servicesFast.end()) {
			return it->second;
		}
		return nullptr;
	}

	void* Engine::getSystemRaw(const std::type_info& type) const
	{
		auto it = impl->systemsFast.find(std::type_index(type));
		if (it != impl->systemsFast.end()) {
			return it->second;
		}
		return nullptr;
	}

	bool Engine::bootstrapResources() const
	{
		return impl->assetsService->loadDefaultAssets();
	}

	Engine::~Engine()
	{
		triple::log::Logger::Info("Engine stoping...");
		delete impl;
	}
}