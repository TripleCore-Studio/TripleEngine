#include "Application/Application.h"
#include "Application/GlWindow.h"
#include "Application/EventSystem.h"
#include "Modules/OpenGLModule.h"
#include "Interfaces/IOpenGLRenderer.h"
#include "Interfaces/IRenderer.h"
#include "TLogger.h"

#include <thread>
#include <chrono>

#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"

namespace TripleEngineCore {
	Application::Application()
	{
		TripleLogger::TLogger::Info("Application starting...");
		this->_pEventDispatcher = std::make_unique<EventDispatcher>();
		this->_pModuleLoader = std::make_unique<System::ModuleLoader>("modules");
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

		std::unique_ptr<Scene::SceneObject> obj = std::make_unique<Scene::SceneObject>();
		obj->addComponent<Scene::TransformComponent>(TripleMath::Vec3(0, 2, 5), TripleMath::Vec3(0, 0, 0), TripleMath::Vec3(1, 1, 1));
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
		float t = this->_pWindow->getTime();
		this->_pScene->rootObjects[0]->getComponent<Scene::TransformComponent>()->rotationEuler 
			= TripleMath::Vec3((sin(t) * 0.5 + 0.5) * 360.0f,cos(t) * 0.5 + 0.5, sin(t) * 0.5 + 0.5);

		this->_pScene->rootObjects[1]->getComponent<Scene::TransformComponent>()->rotationEuler
			= TripleMath::Vec3((sin(t) * 0.5 + 0.5) * 360.0f, tan(t) * 0.5 + 0.5, (sin(t) * 0.5 + 0.5) * 360.0f);

		this->_pScene->rootObjects[2]->getComponent<Scene::TransformComponent>()->rotationEuler
			= TripleMath::Vec3((sin(t) * 0.5 + 0.5), (cos(t) * 0.5 + 0.5) * 360.0f, sin(t) * 0.5 + 0.5);


		this->_pScene->rootObjects[1]->getComponent<Scene::TransformComponent>()->position
			= TripleMath::Vec3(sin(t) * 0.5 + 0.5, cos(t) * 2.5, sin(t) * 2.5f);

		float radius = 6.0f;
		TripleMath::Vec3 camPos(sin(t * 0.5f) * radius, 3.0f, (cos(t * 0.5f) * radius) * ((sin(t) * 0.5 + 0.5) * 5.0));
		this->_pScene->rootObjects[3]->getComponent<Scene::TransformComponent>()->position = camPos;

		this->_pScene->rootObjects[3]->getComponent<Scene::CameraComponent>()->lookAt(TripleMath::Vec3(0,0,0));

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	void Application::onRender()
	{
		float time = this->_pWindow->getTime();
		Graphics::FrameContext ctx;
		for (auto& com : this->_pScene->getAllComponents<Scene::CameraComponent>()) {
			ctx.cameras.push_back(CameraData{com->getViewMatrix(), com->getProjectionMatrix()});
		}
		this->_pScene->gatherRenderCommands(ctx.commands);
		ctx.time = time;

		this->_pRenderer->BeginFrame(time);
		this->_pRenderer->RenderFrame(ctx);
		this->_pRenderer->EndFrame();
	}

	void Application::AddCubeToScene(TripleMath::Vec3 pos, TripleMath::Vec3 size)
	{
		using namespace TripleMath;
		for (int i = 1; i <= 3; i++) {
			std::unique_ptr<Scene::SceneObject> obj = std::make_unique<Scene::SceneObject>();
			obj->addComponent<Scene::TransformComponent>(Vec3(-4 + (i * 2.5), i * 0.1f, 0), Vec3(20, 45, 0), Vec3(1, 1, 1));
			obj->addComponent<Scene::MeshComponent>(Graphics::Model::CreateCube());
			_pScene->addRootObject(std::move(obj));
		}
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