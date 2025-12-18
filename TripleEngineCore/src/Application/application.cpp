#include "Application/Application.h"

#include "Core/CoreTypes.h"

#include "Application/GlWindow.h"
#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "Interfaces/IRenderer.h"
#include "TLogger.h"

#include <thread>
#include <chrono>
#include <iostream>

#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"

namespace TripleEngineCore {
	Application::Application()
	{
		TripleLogger::TLogger::Info("Application starting...");
		this->_pEventDispatcher = std::make_unique<EventDispatcher>();
		this->_pModuleLoader = std::make_unique<System::ModuleLoader>("modules");
		this->_pAssetsSystem = std::make_unique<System::AssetsSystem>();
		this->_pRenderSystem = std::make_unique<System::RenderSystem>(this->_pAssetsSystem.get());
		this->_pScene = std::make_unique<Scene::Scene>();

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

		std::unique_ptr<Scene::SceneObject> obj = std::make_unique<Scene::SceneObject>("camera");
		obj->addComponent<Scene::TransformComponent>(TripleMath::Vec3(0, 3, 20), TripleMath::Vec3(0, 0, 0), TripleMath::Vec3(1, 1, 1));
		obj->addComponent<Scene::CameraComponent>(70.0f, 0.1f, 100.0f, 16.0f / 9.0f, obj->getComponent<Scene::TransformComponent>());
		this->_pScene->addRootObject(std::move(obj));

		while (_isRunning) {
			this->onUpdate();
			this->onRender();
			this->_pWindow->onUpdate();
		}

		return ErrorCode::None;
	}

	void Application::onUpdate()
	{
		Scene::SceneObject* carObj = this->_pScene->findObjectsByName("model1")[0];
		Scene::TransformComponent* transform1 = carObj->getComponent<Scene::TransformComponent>();
		if (!transform1) return;

		static float angle = 0.0f;
		static const float radius = 10.0f;
		static const TripleMath::Vec3 center{ 0, 0, 0 };
		static const float speed = 2.5f;

		angle += speed * 0.01f;
		if (angle > 2 * 3.14159265f) angle -= 2 * 3.14159265f;

		transform1->position.x = center.x + radius * cos(angle);
		transform1->position.z = center.z + radius * sin(angle);
		transform1->position.y = center.y;

		TripleMath::Vec3 forward1 = { -sin(angle), 0.0f, cos(angle) };
		transform1->rotationEuler.y = atan2(forward1.x, forward1.z) * 180.0f / 3.14159265f;


		float t = this->_pWindow->getTime();
		Scene::SceneObject* cameraObj = this->_pScene->findObjectsByName("camera")[0];

		Scene::TransformComponent* transform = cameraObj->getComponent<Scene::TransformComponent>();

		float yaw = transform->rotationEuler.y;
		float pitch = transform->rotationEuler.x;

		float yawRad = yaw * 3.14159265f / 180.0f;

		TripleMath::Vec3 forward(
			sin(yawRad),
			0.0f,
			cos(yawRad)
		);

		TripleMath::Vec3 right(
			cos(yawRad),
			0.0f,
			-sin(yawRad)
		);

		TripleMath::Vec3 up(0, 1, 0);
		TripleMath::Vec3 delta(0, 0, 0);

		if (_keys[Event::KeyCode::W]) delta -= forward;
		if (_keys[Event::KeyCode::S]) delta += forward;
		if (_keys[Event::KeyCode::A]) delta -= right;
		if (_keys[Event::KeyCode::D]) delta += right;
		if (_keys[Event::KeyCode::Space]) delta += up;
		if (_keys[Event::KeyCode::LeftShift]) delta -= up;

		transform->position += delta * _cameraSpeed * 0.01f;

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}


	void Application::onRender()
	{
		float time = this->_pWindow->getTime();
		Graphics::FrameContext ctx;
		for (auto& com : this->_pScene->getAllComponents<Scene::CameraComponent>()) {
			ctx.cameras.push_back(CameraData{com->getViewMatrix(), com->getProjectionMatrix()});
		}
		this->_pRenderSystem->buildRenderCommands(*this->_pScene.get(), ctx.commands);
		ctx.time = time;

		this->_pRenderer->BeginFrame(time);
		this->_pRenderer->RenderFrame(ctx);
		this->_pRenderer->EndFrame();
	}

	void Application::AddModelToScene()
	{
		using namespace TripleMath;

		std::unique_ptr<Scene::SceneObject> obj = std::make_unique<Scene::SceneObject>("model1");
		obj->addComponent<Scene::TransformComponent>(Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(2.0, 2.0, 2.0));
		Index model = this->_pAssetsSystem->loadModel("cube", Graphics::Model::CreateCube());
		obj->addComponent<Scene::MeshComponent>(model);
		this->_pScene->addRootObject(std::move(obj));
	}

	void Application::loadCallbacks()
	{
		this->_pWindow->setEventCallback([this](Event& event) {
			this->_pEventDispatcher->dispatch(event);
		});

		this->_pEventDispatcher->addListener(Event::Type::WindowClose, [this](Event& event) {
			auto& closeEvent = static_cast<WindowCloseEvent&>(event);
			TripleLogger::TLogger::Warn("Window ({}) closed", closeEvent.getTitle());
			this->_pWindow->shutdown();
			this->_isRunning = false;
		});

		this->_pEventDispatcher->addListener(Event::Type::WindowResize, [this](Event& event) {
			auto& resizeEvent = static_cast<WindowResizeEvent&>(event);
			this->getRenderer()->SetViewport(0, 0, resizeEvent.getWidth(), resizeEvent.getHeight());
		});

		this->_pEventDispatcher->addListener(Event::Type::KeyboardInput, [this](Event& event) {
			auto& keyboardEvent = static_cast<KeyboardInputEvent&>(event);
			if (keyboardEvent.isPressed()) {
				KeyClicked(keyboardEvent.getKey());
				_keys[keyboardEvent.getKey()] = true;
			}
			else if (keyboardEvent.isReleased())
				_keys[keyboardEvent.getKey()] = false;
		});
	}

	void Application::KeyClicked(Event::KeyCode key)
	{
		if (key == Event::KeyCode::F1) {
			_cameraSpeed += 1.0;
		}
		else if (key == Event::KeyCode::F2) {
			_cameraSpeed -= 1.0;
			if (_cameraSpeed < 1.0f) _cameraSpeed = 1.0f;
		}
	}

	Application::~Application()
	{
		TripleLogger::TLogger::Info("Application stoping...");
	}
}