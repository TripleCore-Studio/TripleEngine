// OpenGLRenderModule.h
#ifndef OPENGL_RENDER_MODULE_H
#define OPENGL_RENDER_MODULE_H

#include "Interfaces/IModule.h"
#include "Interfaces/IRenderer.h"
#include "DynamicLibrary.h"

namespace TripleEngineCore {
	class OpenGLRenderModule : public IModule {
	public:
		OpenGLRenderModule(const std::string& modulesPath, const std::string& moduleName);
		virtual const std::string& getName() const override;
		virtual bool load() override;
		virtual void unload() override;

		IRenderer* getRenderer() const;

		~OpenGLRenderModule();

	private:
		struct ModuleData {
			LibHandle libHandle = nullptr;
			CreateRendererFunc createFunc = nullptr;
			DestroyRendererFunc destroyFunc = nullptr;
			IRenderer* renderer = nullptr;
		};

		ModuleData _data;
		std::string _path;
		std::string _name;

		// Запрещаем копирование
		OpenGLRenderModule(const OpenGLRenderModule&) = delete;
		OpenGLRenderModule& operator=(const OpenGLRenderModule&) = delete;
	};
}

#endif
