#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef _WIN32
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif

#include <memory>

namespace TripleEngineCore {
	class CORE_API Application {
	public:
		Application();
		virtual ~Application();
		virtual int start(const char* title, unsigned int width, unsigned int height);
		virtual void onUpdate();
	private:
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		std::unique_ptr<struct GLWindow> _pWindow;
	};
}
#endif // APPLICATION_H