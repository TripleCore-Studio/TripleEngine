#include "triple/core/platform/ModuleService.h"

#include <triple/log/Logger.h>

#include "triple/core/platform/OpenGLModule.h"

namespace triple::core {
	ModuleService::ModuleService(std::string modulesPath) { this->m_modulesPath = modulesPath; }
	ModuleService::ErrorCode ModuleService::loadModule(ModuleType type) {
		switch (type) {
		case ModuleType::OpenGLRenderer: {
			if (m_modules.find(type) != m_modules.end()) {
				triple::log::Logger::ModuleError(this->getLoaderClassName(),
				                                 "Module already loaded: OpenGLRenderer");
				return ErrorCode::ModuleAlreadyLoaded;
			}
			auto module = std::make_unique<OpenGLRenderModule>(this->m_modulesPath,
			                                                   TRIPLE_OPENGL_MODULE_FILENAME);
			if (!module->load()) {
				return ErrorCode::FailedToLoadOpenGL;
			}
			m_modules[type] = std::move(module);
			return ErrorCode::None;
			break;
		}
		default:
			triple::log::Logger::ModuleError(this->getLoaderClassName(), "Unknown module type: {}",
			                                 "ModuleType::OpenGLRenderer");
			break;
		}
	}
	void ModuleService::unloadModule(ModuleType type) {
		auto it = m_modules.find(type);
		if (it != m_modules.end()) {
			it->second->unload();
			m_modules.erase(it);
			triple::log::Logger::ModuleInfo(this->getLoaderClassName(),
			                                "Module unloaded successfully: \"{}\"",
			                                static_cast<int>(type));
		} else {
			triple::log::Logger::ModuleError(this->getLoaderClassName(), "Module not found: \"{}\"",
			                                 "ModuleType::OpenGLRenderer");
		}
	}
	IModule *ModuleService::getModule(ModuleType type) {
		auto it = m_modules.find(type);
		if (it != m_modules.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	std::string ModuleService::getLoaderClassName() { return "ModuleLoader"; }
	ModuleService::~ModuleService() {
		for (auto &pair : m_modules) {
			pair.second->unload();
		}
		m_modules.clear();
	}
} // namespace triple::core