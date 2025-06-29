#include "ModuleLoader.h"
#include "DynamicLibrary.h"
#include "TLogger.h"
#include "Modules/OpenGLModule.h"

namespace TripleEngineCore {
	namespace System {
		ModuleLoader::ModuleLoader(std::string modulesPath)
		{
			this->_modulesPath = std::move(modulesPath);
		}
		ModuleLoader::ErrorCode ModuleLoader::loadModule(ModuleType type)
		{
			switch (type)
			{
			case ModuleType::OpenGLRenderer:
			{
				if (_modules.find(type) != _modules.end()) {
					TripleLogger::TLogger::Error("Module already loaded: OpenGLRenderer");
					return ErrorCode::ModuleAlreadyLoaded;
				}
				auto module = std::make_unique<OpenGLRenderModule>(this->_modulesPath, "TripleRenderer_OpenGL");
				if (!module->load()) {
					return ErrorCode::FailedToLoadOpenGL;
				}
				_modules[type] = std::move(module);
				return ErrorCode::None;
				break;
			}
			default:
				TripleLogger::TLogger::Error("Unknown module type: {}", static_cast<int>(type));
				break;
			}
		}
		void ModuleLoader::unloadModule(ModuleType type)
		{
			auto it = _modules.find(type);
			if (it != _modules.end()) {
				it->second->unload();
				_modules.erase(it);
				TripleLogger::TLogger::Info("Module unloaded successfully: \"{}\"", static_cast<int>(type));
			}
			else {
				TripleLogger::TLogger::Error("Module not found: \"{}\"", static_cast<int>(type));
			}
		}
		IModule* ModuleLoader::getModule(ModuleType type)
		{
			auto it = _modules.find(type);
			if (it != _modules.end()) {
				return it->second.get();
			}
			TripleLogger::TLogger::Error("Module not found: \"{}\"", static_cast<int>(type));
			return nullptr;
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