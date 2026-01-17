#include "Modules/OpenGLModule.h"
#include "TLogger.h"

namespace TripleEngineCore {
    OpenGLRenderModule::OpenGLRenderModule(const std::string& modulesPath, const std::string& moduleName)
    {
		this->_name = moduleName;
#ifdef _WIN32
		this->_path = modulesPath + "\\" + moduleName;
#else
		this->_path = modulesPath + "/" + moduleName;
#endif // _WIN32
    }

    const std::string& TripleEngineCore::OpenGLRenderModule::getModuleName() const
    {
        return this->_name;
    }

    bool TripleEngineCore::OpenGLRenderModule::load()
    {
		this->_data.libHandle = IO::DynamicLibrary::Load(this->_path);
        if (!this->_data.libHandle) {
            TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to load OpenGL module: \"{}\"", this->_path);
			return false;
        }

		this->_data.createFunc = reinterpret_cast<CreateRendererFunc>(IO::DynamicLibrary::GetSymbol(this->_data.libHandle, "CreateRenderer"));
        if (!this->_data.createFunc) {
			TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to find CreateRenderer function in OpenGL module: \"{}\"", this->_path);
            this->unload();
            return false;
        }

		this->_data.destroyFunc = reinterpret_cast<DestroyRendererFunc>(IO::DynamicLibrary::GetSymbol(this->_data.libHandle, "DestroyRenderer"));
        if (!this->_data.destroyFunc) {
            TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to find DestroyRenderer function in OpenGL module: \"{}\"", this->_path);
            this->unload();
            return false;
        }

		this->_data.renderer = this->_data.createFunc();
		if (!this->_data.renderer) {
			TripleLogger::TLogger::ModuleError(this->getModuleClassName(), "Failed to create OpenGL renderer from module: \"{}\"", this->_path);
            this->unload();
			return false;
		}
		TripleLogger::TLogger::ModuleInfo(this->getModuleClassName(), "OpenGL module loaded successfully: \"{}\"", this->_path);
		return true;
    }

    void OpenGLRenderModule::unload()
    {
		if (this->_data.destroyFunc && this->_data.renderer) {
			this->_data.destroyFunc(this->_data.renderer);
			this->_data.renderer = nullptr;
		}
		if (this->_data.libHandle) {
			IO::DynamicLibrary::Unload(this->_data.libHandle);
			this->_data.libHandle = nullptr;
            TripleLogger::TLogger::ModuleWarn(this->getModuleClassName(), "OpenGL module unloaded: \"{}\"", this->_path);
		}
    }

    IRenderer* OpenGLRenderModule::getRenderer() const
    {
        return this->_data.renderer;
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
