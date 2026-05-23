#ifndef MODULE_SERVICE_H
#define MODULE_SERVICE_H

#include <type_traits>
#include <functional>

enum class ModuleType { OpenGLRenderer = 0 };

namespace std {
	template <>
	struct hash<ModuleType> {
		size_t operator()(const ModuleType &key) const noexcept {
			return std::hash<std::underlying_type_t<ModuleType>>{}(
			    static_cast<std::underlying_type_t<ModuleType>>(key));
		}
	};
} // namespace std

#include <memory>
#include <unordered_map>
#include "triple/core/base/IModule.h"
#include "triple/core/base/IService.h"

namespace triple::core {
	class ModuleService : public IService {
	  public:
		enum class ErrorCode { None, FailedToLoadOpenGL, ModuleAlreadyLoaded, UnknownModuleType };

		ModuleService(std::string modulesPath);
		ErrorCode loadModule(ModuleType type);
		void unloadModule(ModuleType type);
		IModule *getModule(ModuleType type);
		std::string getLoaderClassName();

		~ModuleService();

	  private:
		ModuleService(const ModuleService &) = delete;
		ModuleService &operator=(const ModuleService &) = delete;
		ModuleService(ModuleService &&) = delete;
		ModuleService &operator=(ModuleService &&) = delete;

		std::unordered_map<ModuleType, std::unique_ptr<IModule>> m_modules;
		std::string m_modulesPath;
	};
} // namespace triple::core
#endif
