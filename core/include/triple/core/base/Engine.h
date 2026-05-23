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
		~Engine();

		bool init();
		ErrorCode run(const char* title, unsigned int width, unsigned int height);
		virtual void onUpdate(float dt);
		void onRender(float t);

		Scene* getActiveScene();
		IWindow* getWindow();
		void setActiveCamera(Entity camera);

		template<typename T>
		T* getService(){
			return static_cast<T*>(getServiceRaw(typeid(T)));
		}

		template<typename T>
		T* getSystem() {
			return static_cast<T*>(getSystemRaw(typeid(T)));
		}

		struct Impl;
		Impl* m_impl;
	private:
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;

		bool bootstrapResources();
		bool bootstrapComponents();

		void loadSystemCallbacks();
		void loadAssetsCallbacks();

		const char* getModuleName() const { return "Engine"; }

		void* getServiceRaw(const std::type_info& type);
		void* getSystemRaw(const std::type_info& type);

		bool m_isInitialized;
		bool m_isRunning;
		float m_lastTime;
	};
}
#endif // APPLICATION_H