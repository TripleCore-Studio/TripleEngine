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
#include <fmt/format.h>

namespace TripleLogger {
    enum class LogLevel {
        Debug,
        Info,
        Warn,
        Error,
		Critical
    };

	class LOGGER_API TLogger
	{
    public:
        template<typename... Args>
        static void Info(const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Info, fmt::format(fmt_str, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Warn(const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Warn, fmt::format(fmt_str, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Error(const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Error, fmt::format(fmt_str, std::forward<Args>(args)...));
        }

		template<typename... Args>
		static void Critical(const std::string& fmt_str, Args&&... args) {
			LogMessage(LogLevel::Critical, fmt::format(fmt_str, std::forward<Args>(args)...));
		}


        template<typename... Args>
        static void ModuleInfo(const std::string& moduleName, const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Info, fmt::format("[" + moduleName + "] -> " + fmt_str, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void ModuleWarn(const std::string& moduleName, const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Warn, fmt::format("[" + moduleName + "] -> " + fmt_str, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void ModuleError(const std::string& moduleName, const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Error, fmt::format("[" + moduleName + "] -> " + fmt_str, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void ModuleCritical(const std::string& moduleName, const std::string& fmt_str, Args&&... args) {
            LogMessage(LogLevel::Critical, fmt::format("[" + moduleName + "] -> " + fmt_str, std::forward<Args>(args)...));
        }
    private:
        static void LogMessage(LogLevel level, const std::string& message);
	};
}
