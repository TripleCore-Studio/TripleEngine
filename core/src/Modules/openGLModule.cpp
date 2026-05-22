#include "triple/core/Modules/OpenGLModule.h"
#include <triple/log/Logger.h>

namespace triple::core {
    OpenGLRenderModule::OpenGLRenderModule(const std::string& modulesPath, const std::string& moduleName)
    {
		this->m_name = moduleName;
#ifdef _WIN32
		this->m_path = modulesPath + "\\" + moduleName;
#else
		this->_path = modulesPath + "/" + moduleName;
#endif // _WIN32
    }

    const std::string& OpenGLRenderModule::getModuleName() const
    {
        return this->m_name;
    }

    bool OpenGLRenderModule::load()
    {
		this->m_data.libHandle = DynamicLibrary::Load(this->m_path);
        if (!this->m_data.libHandle) {
            triple::log::Logger::ModuleError(this->getModuleClassName(), "Failed to load OpenGL module: \"{}\"", this->m_path);
			return false;
        }

		this->m_data.createFunc = reinterpret_cast<gfx::CreateRendererFunc>(DynamicLibrary::GetSymbol(this->m_data.libHandle, "CreateRenderer"));
        if (!this->m_data.createFunc) {
            triple::log::Logger::ModuleError(this->getModuleClassName(), "Failed to find CreateRenderer function in OpenGL module: \"{}\"", this->m_path);
            this->unload();
            return false;
        }

		this->m_data.destroyFunc = reinterpret_cast<gfx::DestroyRendererFunc>(DynamicLibrary::GetSymbol(this->m_data.libHandle, "DestroyRenderer"));
        if (!this->m_data.destroyFunc) {
            triple::log::Logger::ModuleError(this->getModuleClassName(), "Failed to find DestroyRenderer function in OpenGL module: \"{}\"", this->m_path);
            this->unload();
            return false;
        }

		this->m_data.renderer = this->m_data.createFunc();
		if (!this->m_data.renderer) {
            triple::log::Logger::ModuleError(this->getModuleClassName(), "Failed to create OpenGL renderer from module: \"{}\"", this->m_path);
            this->unload();
			return false;
		}
		triple::log::Logger::ModuleInfo(this->getModuleClassName(), "OpenGL module loaded successfully: \"{}\"", this->m_path);
		return true;
    }

    void OpenGLRenderModule::unload()
    {
		if (this->m_data.destroyFunc && this->m_data.renderer) {
			this->m_data.destroyFunc(this->m_data.renderer);
			this->m_data.renderer = nullptr;
		}
		if (this->m_data.libHandle) {
			DynamicLibrary::Unload(this->m_data.libHandle);
			this->m_data.libHandle = nullptr;
            triple::log::Logger::ModuleWarn(this->getModuleClassName(), "OpenGL module unloaded: \"{}\"", this->m_path);
		}
    }

    gfx::IRenderer* OpenGLRenderModule::getRenderer() const
    {
        return this->m_data.renderer;
    }

    std::string OpenGLRenderModule::getModuleClassName()
    {
        return "OpenGLRenderModule";
    }

    OpenGLRenderModule::~OpenGLRenderModule()
    {
		this->unload();
    }
}
