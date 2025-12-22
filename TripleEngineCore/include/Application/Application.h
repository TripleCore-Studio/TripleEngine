#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <unordered_map>
#include <ExportMacros.h>
#include "Interfaces/IRenderer.h"
#include "ModuleLoader.h"
#include "Scene/Scene.h"
#include "Application/EventSystem.h"
#include "System/AssetsSystem.h"
#include "System/RenderSystem.h"

namespace TripleEngineCore {
	class CORE_API Application {
	public:
		enum class ErrorCode {
			None = 0,
			FailedToLoadWindow,
			ModuleLoadError,
			FailedInitRenderer
		};

		Application();
		virtual ~Application();
		virtual ErrorCode start(const char* title, unsigned int width, unsigned int height);
		virtual void onUpdate();
		virtual void onRender();

		void DemoScene();
		void BootstrapResources();

		IRenderer* getRenderer() { return _pRenderer; }
		const IRenderer* getRenderer() const { return _pRenderer; }
	private:
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		void loadCallbacks();
		const char* getModuleName() const { return "Application"; }
		void KeyClicked(Event::KeyCode key);

		std::unique_ptr<System::ModuleLoader> _pModuleLoader;
		std::unique_ptr<struct GLWindow> _pWindow;
		std::unique_ptr<struct EventDispatcher> _pEventDispatcher;

		std::unique_ptr<System::AssetsSystem> _pAssetsSystem;
		std::unique_ptr<System::RenderSystem> _pRenderSystem;

		std::unique_ptr<Scene::Scene> _pScene;

		IRenderer* _pRenderer;
		bool _isRunning;

		std::unordered_map<Event::KeyCode, bool> _keys;
		float _cameraSpeed = 8.0f;
	};
}
#endif // APPLICATION_H