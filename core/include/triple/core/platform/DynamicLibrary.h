#ifndef DYNAMIC_LIBRARY_H
#define DYNAMIC_LIBRARY_H

#include <string>

#if defined(_WIN32)
#include <windows.h>
using LibHandle = HMODULE;
#else
#include <dlfcn.h>
using LibHandle = void *;
#endif

namespace triple::core {
	class DynamicLibrary {
	  public:
		static LibHandle Load(const std::string &path);
		static void Unload(LibHandle handle);
		static void *GetSymbol(LibHandle handle, const std::string &name);
	};
} // namespace triple::core

#endif // DYNAMIC_LIBRARY_H