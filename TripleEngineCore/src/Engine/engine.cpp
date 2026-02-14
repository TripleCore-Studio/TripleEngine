#include "Engine/Engine.h"

#include <thread>
#include <chrono>

#include "Core/CoreTypes.h"

#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "TLogger.h"
#include "Utils/CameraUtils.h"
#include "Utils/TransformUtils.h"
#include "Engine/GLWindow.h"

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

namespace TripleEngineCore {
	Engine::Engine()
	{
		TripleLogger::TLogger::Info("Engine starting...");
		this->_pEventDispatcher = std::make_unique<EventDispatcher>();
		this->_pComponentManager = std::make_unique<ComponentManager>();

		this->_pModuleLoader = std::make_unique<System::ModuleLoader>("modules");

		this->_pAssetsSystem = std::make_unique<System::AssetsSystem>();
		this->_pRenderSystem = std::make_unique<System::RenderSystem>(this->_pAssetsSystem.get());
		this->_pInputSystem = std::make_unique<System::InputSystem>();
		this->_pInputActionSystem = std::make_unique<System::InputActionSystem>(this->_pInputSystem.get());

		this->_pScene = std::make_unique<Scene::Scene>(_pComponentManager.get());

		this->_pModuleLoader->loadModule(ModuleType::OpenGLRenderer);

		this->_isRunning = false;
		this->_lastTime = 0.0f;
		this->_cameraEntity = Scene::INVALID_ENTITY;
	}

	Engine::ErrorCode Engine::start(const char* title, unsigned int width, unsigned int height)
	{
		OpenGLRenderModule* openGLModule = dynamic_cast<OpenGLRenderModule*>(this->_pModuleLoader->getModule(ModuleType::OpenGLRenderer));
		if (openGLModule == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL module");
			return ErrorCode::ModuleLoadError;
		}

		IRenderer* renderer = openGLModule->getRenderer();
		if (renderer == nullptr) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to get OpenGL renderer from module");
			return ErrorCode::ModuleLoadError;
		}
		_pRenderSystem->setRenderer(renderer);

		IOpenGLRenderer* pGLRenderer = dynamic_cast<IOpenGLRenderer*>(renderer);
		if (!pGLRenderer) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to cast renderer to OpenGL renderer interface");
			return ErrorCode::ModuleLoadError;
		}

		auto window = std::make_unique<GLWindow>(title, width, height, _pEventDispatcher.get());
		void* loader = nullptr;
		if (window->init(&loader) != GLWindow::ErrorCode::None) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Failed to initialize window");
			return ErrorCode::FailedToLoadWindow;
		}
		this->_pWindow = std::move(window);

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
		_pInputSystem->init();

		loadSystemCallbacks();
		loadAssetsCallbacks();

		if (!bootstrapResources()) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error: bootstrapResources");
			_pWindow->shutdown();
			return ErrorCode::FailedBootstrapResources;
		}
		if (!bootstrapComponents()) {
			TripleLogger::TLogger::ModuleCritical(this->getModuleName(), "Error: bootstrapComponents");
			_pWindow->shutdown();
			return ErrorCode::FailedBootstrapComponents;
		}
		_pEventDispatcher->dispatch(Event::EngineLoadedEvent());

		_lastTime = _pWindow->getTime();
		this->_isRunning = true;
		while (_isRunning) {
			float currentTime = _pWindow->getTime();
			float dt = currentTime - _lastTime;
			_lastTime = currentTime;

			this->_pWindow->PollEvents();
			this->_pWindow->SwapBuffers();

			this->onUpdate(dt);

			this->_pInputActionSystem->update(dt);
			this->_pInputSystem->update(dt);

			this->onRender(_lastTime);
		}

		return ErrorCode::None;
	}

	void Engine::onUpdate(float dt)
	{
	}

	void Engine::onRender(float t)
	{
		Scene::Scene* scene = this->getActiveScene();
		auto* transform = scene->getComponent<Scene::TransformComponent>(_cameraEntity);
		auto* cameraComp = scene->getComponent<Scene::CameraComponent>(_cameraEntity);

		if (!transform || !cameraComp) {
			TripleLogger::TLogger::ModuleWarn("Engine", "Active camera missing on render");
			return;
		}

		Graphics::FrameContext ctx;

		ctx.camera = Graphics::CameraData{ Utils::getViewMatrix(*transform),
			Utils::getProjectionMatrix(*cameraComp), transform->position};

		this->_pRenderSystem->buildRenderCommands(this->_pScene.get(), ctx.commands);
		ctx.time = t;

		_pRenderSystem->getRenderer()->BeginFrame(t);
		_pRenderSystem->getRenderer()->RenderFrame(ctx);
		_pRenderSystem->getRenderer()->EndFrame();
	}

	bool Engine::bootstrapComponents()
	{
		std::vector<Scene::ComponentTypeID> ids;
		ids.push_back(_pComponentManager->registerComponent<Scene::TransformComponent>());
		ids.push_back(_pComponentManager->registerComponent<Scene::MeshComponent>());
		ids.push_back(_pComponentManager->registerComponent<Scene::CameraComponent>());
		ids.push_back(_pComponentManager->registerComponent<Scene::ParentComponent>());
		ids.push_back(_pComponentManager->registerComponent<Scene::ChildrenComponent>());
		ids.push_back(_pComponentManager->registerComponent<Scene::NameComponent>());

		for (auto& id : ids) {
			if (id == Scene::INVALID_COMPONENT_TYPE_ID) {
				return false;
			}
		}

		return true;
	}

	void Engine::setActiveCamera(const Scene::Entity entity)
	{
		_cameraEntity = entity;
	}

	void Engine::loadSystemCallbacks()
	{
		this->_pEventDispatcher->addListener<Event::WindowCloseEvent>([this](Event::WindowCloseEvent& e) {
			TripleLogger::TLogger::Warn("Window ({}) closed", e.getTitle());
			this->_pWindow->shutdown();
			this->_isRunning = false;
		});

		this->_pEventDispatcher->addListener<Event::WindowResizeEvent>([this](Event::WindowResizeEvent& e) {
			_pRenderSystem->getRenderer()->SetViewport(0, 0, e.getWidth(), e.getHeight());
		});

		this->_pEventDispatcher->addListener<Event::KeyboardInputEvent>([this](Event::KeyboardInputEvent& e) {
			_pInputSystem->onKeyboard(e);
		});

		this->_pEventDispatcher->addListener<Event::MouseMoveEvent>([this](Event::MouseMoveEvent& e) {
			_pInputSystem->onMouseMove(e);
		});

		this->_pEventDispatcher->addListener<Event::MouseButtonEvent>([this](Event::MouseButtonEvent& e) {
			_pInputSystem->onMouseButton(e);
		});
	}

	void Engine::loadAssetsCallbacks()
	{
		_pAssetsSystem->setTextureLoadedCallback([this](const Asset::Texture* tex) {
			_pRenderSystem->uploadTexture(tex);
		});

		_pAssetsSystem->setModelLoadedCallback([this](const Asset::Model* model) {
			_pRenderSystem->uploadGeometry(model);
		});

		_pAssetsSystem->setShaderLoadedCallback([this](const Asset::Shader* shader) {
			_pRenderSystem->uploadShader(shader);
		});
	}

	bool Engine::bootstrapResources()
	{
		return this->_pAssetsSystem->loadDefaultAssets();
	}

	Engine::~Engine()
	{
		TripleLogger::TLogger::Info("Engine stoping...");
	}
}