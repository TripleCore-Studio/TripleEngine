#include "Application/Application.h"

#include <thread>
#include <chrono>

#include "Core/CoreTypes.h"

#include "Application/GlWindow.h"
#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "Interfaces/IRenderer.h"
#include "TLogger.h"

#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"

#include "Asset/Mesh.h"

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

		pGLRenderer->Initialize();

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
		float t = this->_pWindow->getTime();
		Scene::SceneObject* cameraObj = this->_pScene->findObjectsByName("camera")[0];

		Scene::SceneObject* cube = this->_pScene->findObjectsByName("model1")[0];
		cube->getComponent<Scene::TransformComponent>()->position.y = (sin(t * 0.5) * 0.5 + 0.5) * 10.5 + 10;
		cube->getComponent<Scene::TransformComponent>()->rotationEuler.y = t * 20.0f;

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

	void Application::DemoScene()
	{
		using namespace TripleMath;
		using namespace Graphics;

		std::unique_ptr<Scene::SceneObject> cameraObj = std::make_unique<Scene::SceneObject>("camera");
		cameraObj->addComponent<Scene::TransformComponent>(Vec3(0, 2, 23), Vec3(0, 0, 0), Vec3(1, 1, 1));
		cameraObj->addComponent<Scene::CameraComponent>(70.0f, 0.1f, 1000.0f, 16.0f / 9.0f, cameraObj->getComponent<Scene::TransformComponent>());
		this->_pScene->addRootObject(std::move(cameraObj));

		std::unique_ptr<Scene::SceneObject> obj = std::make_unique<Scene::SceneObject>("model1");
		obj->addComponent<Scene::TransformComponent>(Vec3(0, 10, 15), Vec3(0, 30, 0), Vec3(5.0, 5.0, 5.0));
		obj->addComponent<Scene::MeshComponent>(_pAssetsSystem->getModelIndex("cube"));
		this->_pScene->addRootObject(std::move(obj));

		std::unique_ptr<Scene::SceneObject> obj2 = std::make_unique<Scene::SceneObject>("model2");
		obj2->addComponent<Scene::TransformComponent>(Vec3(0, 0, 0), Vec3(-90, 0, 0), Vec3(60.0, 60.0, 2.0));
		obj2->addComponent<Scene::MeshComponent>(_pAssetsSystem->getModelIndex("cube2"));
		this->_pScene->addRootObject(std::move(obj2));
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

	void Application::BootstrapResources()
	{
		this->_pAssetsSystem->loadShaderFromFile("basic_shader",
			"assets/shaders/default/basic_vertex.vs",
			"assets/shaders/default/basic_fragment.fs");

		Asset::AssetID abtex = this->_pAssetsSystem->loadTexture("default_albedo",
			"assets/textures/default/default_albedo.jpg");

		Asset::AssetID abtex2 = this->_pAssetsSystem->loadTexture("brick_albedo",
			"assets/textures/default/default_albedo2.jpeg");

		Asset::AssetID ntex = this->_pAssetsSystem->loadTexture("default_normal",
			"assets/textures/default/default_albedo.jpg");

		Asset::AssetID mttex = this->_pAssetsSystem->loadTexture("default_metallic",
			"assets/textures/default/default_albedo.jpg");

		Asset::AssetID rgtex = this->_pAssetsSystem->loadTexture("default_roughness",
			"assets/textures/default/default_albedo.jpg");

		Asset::Material m;
		m.shaderIndex = this->_pAssetsSystem->getShaderIndex("basic_shader");
		m.albedoColor = TripleMath::Vec3(1.0f, 1.0f, 1.0f);
		m.albedoTextureIndex = abtex;
		m.normalTextureIndex = ntex;
		m.metallicTextureIndex = mttex;
		m.roughnessTextureIndex = rgtex;

		Asset::Material m2;
		m2.shaderIndex = this->_pAssetsSystem->getShaderIndex("basic_shader");
		m2.albedoColor = TripleMath::Vec3(1.0f, 1.0f, 1.0f);
		m2.albedoTextureIndex = abtex2;
		m2.normalTextureIndex = ntex;
		m2.metallicTextureIndex = mttex;
		m2.roughnessTextureIndex = rgtex;

		this->_pAssetsSystem->createMaterial("basic_material", m);
		this->_pAssetsSystem->createMaterial("brick_albedo", m2);

		this->_pAssetsSystem->loadModelFromModel("cube", Asset::Model::CreateCube());
		this->_pAssetsSystem->loadModelFromModel("cube2", Asset::Model::CreateCube());

		auto model = this->_pAssetsSystem->getModelMutable(_pAssetsSystem->getModelIndex("cube"));
		auto model2 = this->_pAssetsSystem->getModelMutable(_pAssetsSystem->getModelIndex("cube2"));
		auto matId = this->_pAssetsSystem->getMaterialIndex("basic_material");
		auto matId2 = this->_pAssetsSystem->getMaterialIndex("brick_albedo");

		for(auto& mesh : model->meshes) {
			if(mesh.materialIndex != System::INVALID_ASSET_ID) continue;
			mesh.materialIndex = matId;
		}

		for (auto& mesh : model2->meshes) {
			if (mesh.materialIndex != System::INVALID_ASSET_ID) continue;
			mesh.materialIndex = matId2;
		}
	}

	Application::~Application()
	{
		TripleLogger::TLogger::Info("Application stoping...");
	}
}