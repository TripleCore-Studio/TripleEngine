#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <unordered_map>

#include <ExportMacros.h>
#include "ModuleLoader.h"
#include "Scene/Scene.h"

#include "Engine/EventSystem.h"

#include "System/AssetsSystem.h"
#include "System/RenderSystem.h"
#include "System/InputSystem.h"

namespace TripleEngineCore {
	class CORE_API Engine {
	public:
		enum class ErrorCode {
			None = 0,
			FailedToLoadWindow,
			ModuleLoadError,
			FailedInitRenderer
		};

		Engine();
		virtual ~Engine();
		virtual ErrorCode start(const char* title, unsigned int width, unsigned int height);
		virtual void onUpdate(float dt);
		virtual void onRender(float t);

		void DemoScene();
		void BootstrapResources();

	private:
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;

		void loadCallbacks();
		const char* getModuleName() const { return "Engine"; }
		void KeyClicked(Event::KeyCode key);

		std::unique_ptr<System::ModuleLoader> _pModuleLoader;
		std::unique_ptr<struct GLWindow> _pWindow;
		std::unique_ptr<struct EventDispatcher> _pEventDispatcher;

		std::unique_ptr<System::AssetsSystem> _pAssetsSystem;
		std::unique_ptr<System::RenderSystem> _pRenderSystem;
		std::unique_ptr<System::InputSystem> _pInputSystem;

		std::unique_ptr<Scene::Scene> _pScene;

		bool _isRunning;
		float _cameraSpeed;
		float lastTime;
	};
}
#endif // APPLICATION_H