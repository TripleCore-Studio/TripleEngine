#ifndef APPLICATION_H
#define APPLICATION_H

#include "triple/core/base/ExportMacros.h"

#include <typeinfo>

#include "triple/core/ecs/Scene.h"
#include "triple/core/base/IWindow.h"

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
		virtual ~Engine();

		bool init();
		ErrorCode run(const char *title, unsigned int width, unsigned int height);
		virtual void onUpdate(float dt);
		virtual void onRenderUI(float dt);
		void onRender(float t);

		[[nodiscard]] Scene *getActiveScene() const;
		[[nodiscard]] IWindow *getWindow() const;
		void setActiveCamera(Entity camera) const;

		template <typename T>
		T *getService() {
			return static_cast<T *>(getServiceRaw(typeid(T)));
		}

		template <typename T>
		T *getSystem() {
			return static_cast<T *>(getSystemRaw(typeid(T)));
		}

		Engine(const Engine &) = delete;
		Engine(Engine &&) = delete;
		Engine &operator=(const Engine &) = delete;
		Engine &operator=(Engine &&) = delete;

		struct Impl;
		Impl *impl;

	private:
		[[nodiscard]] bool bootstrapResources() const;
		[[nodiscard]] bool bootstrapComponents() const;

		void loadSystemCallbacks();
		void loadAssetsCallbacks();

		[[nodiscard]] const char *getModuleName() const { return "Engine"; }

		[[nodiscard]] void *getServiceRaw(const std::type_info &type) const;
		[[nodiscard]] void *getSystemRaw(const std::type_info &type) const;

		bool m_isInitialized;
		bool m_isRunning;
		float m_lastTime;
	};
} // namespace triple::core
#endif // APPLICATION_H