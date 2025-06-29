// IModule.h
#ifndef I_MODULE_H
#define I_MODULE_H

#include <string>

namespace TripleEngineCore {
	class IModule {
	public:
		virtual const std::string& getName() const = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;
		virtual ~IModule() = default;
	};
}

#endif
