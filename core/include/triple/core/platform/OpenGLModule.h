#ifndef OPENGL_RENDER_MODULE_H
#define OPENGL_RENDER_MODULE_H

#include <triple/gfx/IRenderer.h>
#include "triple/core/base/IModule.h"
#include "triple/core/platform/DynamicLibrary.h"

namespace triple::core {
	class OpenGLRenderModule : public IModule {
	  public:
		OpenGLRenderModule(const std::string &modulesPath, const std::string &moduleName);
		virtual const std::string &getModuleName() const override;
		virtual bool load() override;
		virtual void unload() override;

		gfx::IRenderer *getRenderer() const;
		std::string getModuleClassName();

		~OpenGLRenderModule();

	  private:
		struct ModuleData {
			LibHandle libHandle = nullptr;
			gfx::CreateRendererFunc createFunc = nullptr;
			gfx::DestroyRendererFunc destroyFunc = nullptr;
			gfx::IRenderer *renderer = nullptr;
		};

		ModuleData m_data;
		std::string m_path;
		std::string m_name;

		OpenGLRenderModule(const OpenGLRenderModule &) = delete;
		OpenGLRenderModule &operator=(const OpenGLRenderModule &) = delete;
	};
} // namespace triple::core

#endif