#pragma once

#ifdef _WIN32
  #ifdef LOGGER_EXPORTS
    #define LOGGER_API __declspec(dllexport)
  #else
    #define LOGGER_API __declspec(dllimport)
  #endif
#else
  #define LOGGER_API
#endif

#include <string>

namespace TripleLogger {
	class LOGGER_API TLogger
	{
	public:
		static TLogger& Instance() {
			static TLogger instance;
			return instance;
		}

		void LogError(const std::string& text);
		void LogWarning(const std::string& text);
		void LogInfo(const std::string& text);
		void LogCritical(const std::string& text);
	private:
		TLogger() = default;
		~TLogger() = default;
		TLogger(const TLogger&) = delete;
		TLogger& operator=(const TLogger&) = delete;
	};
}
