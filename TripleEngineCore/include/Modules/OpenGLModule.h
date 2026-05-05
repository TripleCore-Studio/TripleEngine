#ifndef OPENGL_RENDER_MODULE_H
#define OPENGL_RENDER_MODULE_H

#include "Interfaces/IModule.h"
#include "Interfaces/IRenderer.h"
#include "IO/DynamicLibrary.h"

namespace TripleEngineCore {
	class OpenGLRenderModule : public IModule {
	public:
		OpenGLRenderModule(const std::string& modulesPath, const std::string& moduleName);
		virtual const std::string& getModuleName() const override;
		virtual bool load() override;
		virtual void unload() override;

		IRenderer* getRenderer() const;
		std::string getModuleClassName();

		~OpenGLRenderModule();

	private:
		struct ModuleData {
			LibHandle libHandle = nullptr;
			CreateRendererFunc createFunc = nullptr;
			DestroyRendererFunc destroyFunc = nullptr;
			IRenderer* renderer = nullptr;
		};

		ModuleData m_data;
		std::string m_path;
		std::string m_name;

		OpenGLRenderModule(const OpenGLRenderModule&) = delete;
		OpenGLRenderModule& operator=(const OpenGLRenderModule&) = delete;
	};
}

#endif
