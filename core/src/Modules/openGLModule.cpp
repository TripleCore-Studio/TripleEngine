#include "Modules/OpenGLModule.h"
#include "TLogger.h"

namespace TripleEngineCore {
    OpenGLRenderModule::OpenGLRenderModule(const std::string& modulesPath, const std::string& moduleName)
    {
		this->m_name = moduleName;
#ifdef _WIN32
		this->m_path = modulesPath + "\\" + moduleName;
#else
		this->_path = modulesPath + "/" + moduleName;
#endif // _WIN32
    }

    const std::string& TripleEngineCore::OpenGLRenderModule::getModuleName() const
    {
        return this->m_name;
    }

    bool TripleEngineCore::OpenGLRenderModule::load()
    {
		this->m_data.libHandle = IO::DynamicLibrary::Load(this->m_path);
        if (!this->m_data.libHandle) {
            TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to load OpenGL module: \"{}\"", this->m_path);
			return false;
        }

		this->m_data.createFunc = reinterpret_cast<CreateRendererFunc>(IO::DynamicLibrary::GetSymbol(this->m_data.libHandle, "CreateRenderer"));
        if (!this->m_data.createFunc) {
			TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to find CreateRenderer function in OpenGL module: \"{}\"", this->m_path);
            this->unload();
            return false;
        }

		this->m_data.destroyFunc = reinterpret_cast<DestroyRendererFunc>(IO::DynamicLibrary::GetSymbol(this->m_data.libHandle, "DestroyRenderer"));
        if (!this->m_data.destroyFunc) {
            TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to find DestroyRenderer function in OpenGL module: \"{}\"", this->m_path);
            this->unload();
            return false;
        }

		this->m_data.renderer = this->m_data.createFunc();
		if (!this->m_data.renderer) {
			TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to create OpenGL renderer from module: \"{}\"", this->m_path);
            this->unload();
			return false;
		}
		TripleLogger::TLogger::ModuleInfo(this->getModuleClassName(), "OpenGL module loaded successfully: \"{}\"", this->m_path);
		return true;
    }

    void OpenGLRenderModule::unload()
    {
		if (this->m_data.destroyFunc && this->m_data.renderer) {
			this->m_data.destroyFunc(this->m_data.renderer);
			this->m_data.renderer = nullptr;
		}
		if (this->m_data.libHandle) {
			IO::DynamicLibrary::Unload(this->m_data.libHandle);
			this->m_data.libHandle = nullptr;
            TripleLogger::TLogger::ModuleWarn(this->getModuleClassName(), "OpenGL module unloaded: \"{}\"", this->m_path);
		}
    }

    IRenderer* OpenGLRenderModule::getRenderer() const
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
