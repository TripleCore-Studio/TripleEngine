#include "triple/core/platform/DynamicLibrary.h"
namespace triple::core {
	LibHandle DynamicLibrary::Load(const std::string &path) {
#if defined(_WIN32)
		return LoadLibraryA(path.c_str());
#else
		return dlopen(path.c_str(), RTLD_NOW);
#endif
	}

	void DynamicLibrary::Unload(LibHandle handle) {
#if defined(_WIN32)
		FreeLibrary(handle);
#else
		dlclose(handle);
#endif
	}

	void *DynamicLibrary::GetSymbol(LibHandle handle, const std::string &name) {
#if defined(_WIN32)
		return reinterpret_cast<void *>(GetProcAddress(handle, name.c_str()));
#else
		return dlsym(handle, name.c_str());
#endif
	}
} // namespace triple::core
