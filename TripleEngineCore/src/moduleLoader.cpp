#include "ModuleLoader.h"
#include "DynamicLibrary.h"
#include "TLogger.h"
#include "Modules/OpenGLModule.h"

namespace TripleEngineCore {
	namespace System {
		ModuleLoader::ModuleLoader(std::string modulesPath)
		{
			this->_modulesPath = modulesPath;
		}
		ModuleLoader::ErrorCode ModuleLoader::loadModule(ModuleType type)
		{
			switch (type)
			{
			case ModuleType::OpenGLRenderer:
			{
				if (_modules.find(type) != _modules.end()) {
					TripleLogger::TLogger::ModuleError(this->getLoaderClassName(), "Module already loaded: OpenGLRenderer");
					return ErrorCode::ModuleAlreadyLoaded;
				}
				auto module = std::make_unique<OpenGLRenderModule>(this->_modulesPath, TRIPLE_OPENGL_MODULE_FILENAME);
				if (!module->load()) {
					return ErrorCode::FailedToLoadOpenGL;
				}
				_modules[type] = std::move(module);
				return ErrorCode::None;
				break;
			}
			default:
				TripleLogger::TLogger::ModuleError(this->getLoaderClassName(), "Unknown module type: {}", "ModuleType::OpenGLRenderer");
				break;
			}
		}
		void ModuleLoader::unloadModule(ModuleType type)
		{
			auto it = _modules.find(type);
			if (it != _modules.end()) {
				it->second->unload();
				_modules.erase(it);
				TripleLogger::TLogger::ModuleInfo(this->getLoaderClassName(), "Module unloaded successfully: \"{}\"", static_cast<int>(type));
			}
			else {
				TripleLogger::TLogger::ModuleError(this->getLoaderClassName(), "Module not found: \"{}\"", "ModuleType::OpenGLRenderer");
			}
		}
		IModule* ModuleLoader::getModule(ModuleType type)
		{
			auto it = _modules.find(type);
			if (it != _modules.end()) {
				return it->second.get();
			}
			return nullptr;
		}
		std::string ModuleLoader::getLoaderClassName()
		{
			return "ModuleLoader";
		}
		ModuleLoader::~ModuleLoader()
		{
			for (auto& pair : _modules) {
				pair.second->unload();
			}
			_modules.clear();
		}
	}
}