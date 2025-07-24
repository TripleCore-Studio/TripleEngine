// ModuleLoader.h
#ifndef MODULE_LOADER_H
#define MODULE_LOADER_H

#include <type_traits>
#include <functional>

enum class ModuleType {
	OpenGLRenderer = 0
};


namespace std {
	template <>
	struct hash<ModuleType> {
		size_t operator()(const ModuleType& key) const noexcept {
			return std::hash<std::underlying_type_t<ModuleType>>{}(
				static_cast<std::underlying_type_t<ModuleType>>(key));
		}
	};
}

#include <memory>
#include <unordered_map>
#include "Interfaces/IModule.h"

namespace TripleEngineCore {
	namespace System {
		class ModuleLoader {
		public:
			enum class ErrorCode {
				None,
				FailedToLoadOpenGL,
				ModuleAlreadyLoaded,
				UnknownModuleType
			};

			ModuleLoader(std::string modulesPath);
			ErrorCode loadModule(ModuleType type);
			void unloadModule(ModuleType type);
			IModule* getModule(ModuleType type);
			std::string getLoaderClassName();

			~ModuleLoader();

		private:
			ModuleLoader(const ModuleLoader&) = delete;
			ModuleLoader& operator=(const ModuleLoader&) = delete;
			ModuleLoader(ModuleLoader&&) = delete;
			ModuleLoader& operator=(ModuleLoader&&) = delete;

			std::unordered_map<ModuleType, std::unique_ptr<IModule>> _modules;
			std::string _modulesPath;
		};
	}
}
#endif
