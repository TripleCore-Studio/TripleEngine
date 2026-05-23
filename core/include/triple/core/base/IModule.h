// IModule.h
#ifndef I_MODULE_H
#define I_MODULE_H

#include <string>

namespace triple::core {
	class IModule {
	  public:
		virtual const std::string &getModuleName() const = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;
		virtual ~IModule() = default;
	};
} // namespace triple::core

#endif
