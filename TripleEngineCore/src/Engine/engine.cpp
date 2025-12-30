#include "Engine/Engine.h"

#include <thread>
#include <chrono>

#include "Core/CoreTypes.h"

#include "Engine/GlWindow.h"
#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "TLogger.h"
#include "MathCommon.h"
#include "Quat.h"

#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"

namespace TripleEngineCore {
	Engine::Engine()
	{
		TripleLogger::TLogger::Info("Engine starting...");
		this->_pEventDispatcher = std::make_unique<EventDispatcher>();
		this->_pModuleLoader = std::make_unique<System::ModuleLoader>("modules");

		this->_pAssetsSystem = std::make_unique<System::AssetsSystem>();
		this->_pRenderSystem = std::make_unique<System::RenderSystem>(this->_pAssetsSystem.get());
		this->_pInputSystem = std::make_unique<System::InputSystem>();

		this->_pScene = std::make_unique<Scene::Scene>();

		this->_pModuleLoader->loadModule(ModuleType::OpenGLRenderer);
		this->_isRunning = false;
		this->lastTime = 0.0f;
		this->_cameraSpeed = 8.0f;
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
		_pInputSystem->init();

		loadCallbacks();
		BootstrapResources();
		DemoScene();

		lastTime = _pWindow->getTime();
		this->_isRunning = true;
		while (_isRunning) {
			float currentTime = _pWindow->getTime();
			float dt = currentTime - lastTime;
			lastTime = currentTime;

			this->_pWindow->onUpdate();
			this->onUpdate(dt);
			this->_pInputSystem->update(dt);
			this->onRender(lastTime);
		}

		return ErrorCode::None;
	}
	bool isWeapon = true;
	void Engine::onUpdate(float dt)
	{
		Scene::SceneObject* cameraObj = this->_pScene->findObjectsByName("camera")[0];
		Scene::TransformComponent* cTransform = cameraObj->getComponent<Scene::TransformComponent>();

		TripleMath::Vec3 delta(0, 0, 0);
		if (_pInputSystem->isKeyDown(Event::KeyCode::W)) delta += cTransform->forward();
		if (_pInputSystem->isKeyDown(Event::KeyCode::S)) delta -= cTransform->forward();
		if (_pInputSystem->isKeyDown(Event::KeyCode::A)) delta -= cTransform->right();
		if (_pInputSystem->isKeyDown(Event::KeyCode::D)) delta += cTransform->right();
		if (_pInputSystem->isKeyDown(Event::KeyCode::Space)) delta += cTransform->up();
		if (_pInputSystem->isKeyDown(Event::KeyCode::LeftShift)) delta -= cTransform->up();
		cTransform->position += delta * _cameraSpeed * dt;

		auto mdelta = _pInputSystem->getMouseDelta();
		cTransform->rotationEuler.y -= mdelta.x * 0.3;
		cTransform->rotationEuler.x += mdelta.y * 0.3;

		cTransform->rotationEuler.x = TripleMath::clamp(
			cTransform->rotationEuler.x,
			-89.0f,
			89.0f
		);

		Scene::SceneObject* weapon = this->_pScene->findObjectsByName("obj")[0];
		Scene::TransformComponent* tWeapon = weapon->getComponent<Scene::TransformComponent>();

		if (_pInputSystem->isKeyPressed(Event::KeyCode::LeftCtrl)) {
			isWeapon = !isWeapon;
			if (isWeapon) {
				weapon->getComponent<Scene::MeshComponent>()->modelIndex = _pAssetsSystem->getModelId("demo_model3");
				tWeapon->scale = TripleMath::Vec3(0.02, 0.02, 0.02);
			}
			else {
				weapon->getComponent<Scene::MeshComponent>()->modelIndex = _pAssetsSystem->getModelId("demo_model");
				tWeapon->scale = TripleMath::Vec3(0.1, 0.1, 0.1);
			}
		}

		TripleMath::Vec3 localOffset;
		if (!_pInputSystem->isMouseButtonDown(Event::MouseButton::Right)) {
			if (isWeapon) {
				localOffset = TripleMath::Vec3(0.6f, -1.2f, 0.8f);
			}
			else {
				localOffset = TripleMath::Vec3(1.4f, -1.2f, 2.2f);
			}
		}
		else {
			if (isWeapon) {
				localOffset = TripleMath::Vec3(0.1f, -1.2f, 0.4f);
			}
			else {
				localOffset = TripleMath::Vec3(0.0f, -0.8f, 1.8f);
			}
		}

		if (_pInputSystem->isMouseButtonDown(Event::MouseButton::Button5)) {
			if (_cameraSpeed < 100) {
				_cameraSpeed++;
			}
		}
		else if (_pInputSystem->isMouseButtonDown(Event::MouseButton::Button4)) {
			if (_cameraSpeed > 1) {
				_cameraSpeed--;
			}
		}

		TripleMath::Vec3 worldOffset = cTransform->right() * localOffset.x +
			cTransform->up() * localOffset.y +
			cTransform->forward() * localOffset.z;

		tWeapon->position = cTransform->position + worldOffset;

		TripleMath::Quat weaponLocalRot;
		if (isWeapon) {
			weaponLocalRot = TripleMath::Quat::fromEulerAngles(TripleMath::Vec3(0.0f, 90.0f, 0.0f));
		}
		else {
			weaponLocalRot = TripleMath::Quat::fromEulerAngles(TripleMath::Vec3(0.0f, 180.0f, 0.0f));
		}
		TripleMath::Quat camRot = TripleMath::Quat::fromEulerAngles(cTransform->rotationEuler);
		TripleMath::Quat weaponWorldRot = camRot * weaponLocalRot;
		tWeapon->rotationEuler = weaponWorldRot.toEulerAngles();

		if (_pInputSystem->isKeyPressed(Event::KeyCode::F11)) {
			_pWindow->toggleFullscreen();
		}
	}


	void Engine::onRender(float t)
	{
		Graphics::FrameContext ctx;
		for (auto& camera : this->_pScene->getAllComponents<Scene::CameraComponent>()) {
			ctx.cameras.push_back(Graphics::CameraData{ camera->getViewMatrix(), camera->getProjectionMatrix(), camera->transform->position });
		}
		this->_pRenderSystem->buildRenderCommands(this->_pScene.get(), ctx.commands);
		ctx.time = t;

		_pRenderSystem->getRenderer()->BeginFrame(t);
		_pRenderSystem->getRenderer()->RenderFrame(ctx);
		_pRenderSystem->getRenderer()->EndFrame();
	}

	void Engine::DemoScene()
	{
		using namespace TripleMath;
		using namespace Graphics;

		std::unique_ptr<Scene::SceneObject> cameraObj = std::make_unique<Scene::SceneObject>("camera");
		cameraObj->addComponent<Scene::TransformComponent>(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(1, 1, 1));
		cameraObj->addComponent<Scene::CameraComponent>(70.0f, 0.1f, 1000.0f, 16.0f / 9.0f, cameraObj->getComponent<Scene::TransformComponent>());

		std::unique_ptr<Scene::SceneObject> weaponObj = std::make_unique<Scene::SceneObject>("obj");
		weaponObj->addComponent<Scene::TransformComponent>(Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0.02, 0.02, 0.02));
		weaponObj->addComponent<Scene::MeshComponent>(_pAssetsSystem->getModelId("demo_model3"));

		std::unique_ptr<Scene::SceneObject> charObj = std::make_unique<Scene::SceneObject>("obj2");
		charObj->addComponent<Scene::TransformComponent>(Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(1, 1, 1));
		charObj->addComponent<Scene::MeshComponent>(_pAssetsSystem->getModelId("demo_model2"));

		std::unique_ptr<Scene::SceneObject> tank = std::make_unique<Scene::SceneObject>("tank");
		tank->addComponent<Scene::TransformComponent>(Vec3(0, 0, 70), Vec3(-90, 0, 0), Vec3(0.8, 0.8, 0.8));
		tank->addComponent<Scene::MeshComponent>(_pAssetsSystem->getModelId("tank"));

		std::unique_ptr<Scene::SceneObject> tank2 = std::make_unique<Scene::SceneObject>("tank2");
		tank2->addComponent<Scene::TransformComponent>(Vec3(0, 0, -70), Vec3(-90, 0, 0), Vec3(0.8, 0.8, 0.8));
		tank2->addComponent<Scene::MeshComponent>(_pAssetsSystem->getModelId("tank"));

		this->_pScene->addRootObject(std::move(weaponObj));
		this->_pScene->addRootObject(std::move(cameraObj));
		this->_pScene->addRootObject(std::move(charObj));
		this->_pScene->addRootObject(std::move(tank));
		this->_pScene->addRootObject(std::move(tank2));
	}

	void Engine::loadCallbacks()
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
			_pRenderSystem->getRenderer()->SetViewport(0, 0, resizeEvent.getWidth(), resizeEvent.getHeight());
		});

		this->_pEventDispatcher->addListener(Event::Type::KeyboardInput, [this](Event& event) {
			_pInputSystem->onEvent(event);
		});

		this->_pEventDispatcher->addListener(Event::Type::MouseMove, [this](Event& event) {
			_pInputSystem->onEvent(event);
		});

		this->_pEventDispatcher->addListener(Event::Type::MouseButtonInput, [this](Event& event) {
			_pInputSystem->onEvent(event);
		});
	}

	void Engine::KeyClicked(Event::KeyCode key)
	{
		if (key == Event::KeyCode::F1) {
			_cameraSpeed += 1.0;
		}
		else if (key == Event::KeyCode::F2) {
			_cameraSpeed -= 1.0;
			if (_cameraSpeed < 1.0f) _cameraSpeed = 1.0f;
		}
	}

	void Engine::BootstrapResources()
	{
		using namespace TripleMath;
		System::TextureID ard = _pAssetsSystem->genSolidTexture("__default_white", 255, 255, 255, 255); // albedo, roughness
		System::TextureID mtd = _pAssetsSystem->genSolidTexture("__default_black", 0, 0, 0, 255); // metallic
		System::TextureID nd = _pAssetsSystem->genSolidTexture("__default_normal", 128, 128, 255, 255); // normal

		System::ShaderID sd = _pAssetsSystem->loadShaderFromFile("__default_shader",
			"assets\\shaders\\__default_shader.vert",
			"assets\\shaders\\__default_shader.frag");

		if (ard == Asset::INVALID_ASSET_ID || mtd == Asset::INVALID_ASSET_ID || nd == Asset::INVALID_ASSET_ID || sd == Asset::INVALID_ASSET_ID) {
			TripleLogger::TLogger::Critical("The default resources were not loaded properly, and the program cannot continue working normally.");
			exit(-1);
		}

		_pRenderSystem->uploadTexture(_pAssetsSystem->getTexture(ard));
		_pRenderSystem->uploadTexture(_pAssetsSystem->getTexture(mtd));
		_pRenderSystem->uploadTexture(_pAssetsSystem->getTexture(nd));
		_pRenderSystem->uploadShader(_pAssetsSystem->getShader(sd));

		Asset::Material mtdd;
		mtdd.albedoColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mtdd.albedoTextureId = ard;
		mtdd.metallicTextureId = mtd;
		mtdd.normalTextureId = nd;
		mtdd.roughnessTextureId = ard;
		mtdd.shaderId = sd;
		mtdd.metallic = 0.1;
		mtdd.roughness = 1.0;
		System::MaterialID mdid = _pAssetsSystem->createMaterial("__default_material", mtdd);
		if (mdid == Asset::INVALID_ASSET_ID) {
			TripleLogger::TLogger::Critical("The default resources were not loaded properly, and the program cannot continue working normally.");
			exit(-1);
		}

		System::ModelID demo_model = _pAssetsSystem->loadModelFromFile("demo_model", "assets\\models\\weapon.glb");
		if (demo_model != Asset::INVALID_ASSET_ID) _pRenderSystem->uploadGeometry(_pAssetsSystem->getModel(demo_model));

		System::ModelID demo_model3 = _pAssetsSystem->loadModelFromFile("demo_model3", "assets\\models\\weapon_1.glb");
		if (demo_model3 != Asset::INVALID_ASSET_ID) _pRenderSystem->uploadGeometry(_pAssetsSystem->getModel(demo_model3));

		System::ModelID demo_model2 = _pAssetsSystem->loadModelFromFile("demo_model2", "assets\\models\\spec.glb");
		if (demo_model2 != Asset::INVALID_ASSET_ID) _pRenderSystem->uploadGeometry(_pAssetsSystem->getModel(demo_model2));

		System::ModelID town_square = _pAssetsSystem->loadModelFromFile("tank", "assets\\models\\tank.glb");
		if (town_square != Asset::INVALID_ASSET_ID) _pRenderSystem->uploadGeometry(_pAssetsSystem->getModel(town_square));
	}

	Engine::~Engine()
	{
		TripleLogger::TLogger::Info("Engine stoping...");
	}
}