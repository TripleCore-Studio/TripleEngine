#ifndef APPLICATION_H
#define APPLICATION_H

#include "triple/core/base/ExportMacros.h"
#include "triple/core/base/IWindow.h"
#include "triple/core/base/LayerStack.h"

namespace triple::core {
	class CORE_API Engine {
	public:
		enum class ErrorCode {
			None = 0,
			FailedToLoadWindow,
			ModuleLoadError,
			FailedInitRenderer,
			FailedInitEngine,
			FailedBootstrapResources,
			FailedBootstrapComponents
		};

		Engine();
		~Engine();

		bool init();
		ErrorCode run(const char *title, unsigned int width, unsigned int height);

		[[nodiscard]] IWindow *getWindow() const;

		void pushLayer(std::unique_ptr<Layer> layer);
		void pushOverlay(std::unique_ptr<Layer> overlay);

		Engine(const Engine &) = delete;
		Engine(Engine &&) = delete;
		Engine &operator=(const Engine &) = delete;
		Engine &operator=(Engine &&) = delete;

	private:
		struct Impl;
		Impl *m_impl;

		void loadSystemCallbacks();
		void dispatchToLayers(Event &e);

		[[nodiscard]] const char *getModuleName() const { return "Engine"; }

		bool m_isInitialized;
		bool m_isRunning;
		float m_lastTime;
		LayerStack m_layerStack;
	};
} // namespace triple::core
#endif // APPLICATION_H