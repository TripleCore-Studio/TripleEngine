#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <ExportMacros.h>
#include "Interfaces/IRenderer.h"
#include "ModuleLoader.h"

namespace TripleEngineCore {
	class CORE_API Application {
	public:
		Application();
		virtual ~Application();
		virtual int start(const char* title, unsigned int width, unsigned int height);
		virtual void onUpdate();
	private:
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		void loadCallbacks();

		std::unique_ptr<System::ModuleLoader> _pModuleLoader;

		std::unique_ptr<struct GLWindow> _pWindow;
		std::unique_ptr<struct EventDispatcher> _pEventDispatcher;
		bool _isRunning;
	};
}
#endif // APPLICATION_H