#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <unordered_map>

#include "ExportMacros.h"
#include "ModuleLoader.h"
#include "Scene/Scene.h"

#include "Engine/EventSystem.h"
#include "Engine/GlWindow.h"
#include "Engine/ComponentManager.h"

#include "System/AssetsSystem.h"
#include "System/RenderSystem.h"
#include "System/InputSystem.h"

#include "Scene/CameraComponent.h"
#include "Scene/TransformComponent.h"

namespace TripleEngineCore {
	class CORE_API Engine {
	public:
		enum class ErrorCode {
			None = 0,
			FailedToLoadWindow,
			ModuleLoadError,
			FailedInitRenderer,
			FailedBootstrapDefaultResources
		};

		Engine();
		virtual ~Engine();
		virtual ErrorCode start(const char* title, unsigned int width, unsigned int height);
		virtual void onUpdate(float dt);
		virtual void onRender(float t) const;

		bool bootstrapResources();
		bool bootstrapComponents();

		void setActiveCamera(Scene::CameraComponent cameraComponent, Scene::TransformComponent transformComponent);

		System::InputSystem* getInputSys() { return _pInputSystem.get(); }
		System::AssetsSystem* getAssetSys() { return _pAssetsSystem.get(); }
		System::RenderSystem* getRenderSys() { return _pRenderSystem.get(); }
		Scene::Scene* getActiveScene() { return _pScene.get(); }
		EventDispatcher* getEventDispatcher() { return _pEventDispatcher.get(); }
		GLWindow* getWindow() { return _pWindow.get(); }
	private:
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;

		void loadCallbacks();
		const char* getModuleName() const { return "Engine"; }

		std::unique_ptr<GLWindow> _pWindow;
		std::unique_ptr<EventDispatcher> _pEventDispatcher;
		std::unique_ptr<ComponentManager> _pComponentManager;
		std::unique_ptr<System::ModuleLoader> _pModuleLoader;
		std::unique_ptr<System::AssetsSystem> _pAssetsSystem;
		std::unique_ptr<System::RenderSystem> _pRenderSystem;
		std::unique_ptr<System::InputSystem> _pInputSystem;
		std::unique_ptr<Scene::Scene> _pScene;

		struct Camera {
			Scene::CameraComponent _cameraComponent;
			Scene::TransformComponent _transformComponent;
		};
		Camera _camera;

		bool _isRunning;
		float _lastTime;
	};
}
#endif // APPLICATION_H