#pragma once

#include <fmt/format.h>

namespace triple::log {
	enum class LogLevel { Debug, Info, Warn, Error, Critical };

	class Logger {
	public:
		template <typename... Args>
		static void info(std::string_view fmtStr, Args &&...args) {
			logMessage(LogLevel::Info, fmt::vformat(fmtStr, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void warn(std::string_view fmtStr, Args &&...args) {
			logMessage(LogLevel::Warn, fmt::vformat(fmtStr, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void error(std::string_view fmtStr, Args &&...args) {
			logMessage(LogLevel::Error, fmt::vformat(fmtStr, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void critical(std::string_view fmtStr, Args &&...args) {
			logMessage(LogLevel::Critical, fmt::vformat(fmtStr, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void moduleInfo(std::string_view moduleName, std::string_view fmtStr,
		                       Args &&...args) {
			logMessage(LogLevel::Info,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmtStr, fmt::make_format_args(args...))));
		}

		template <typename... Args>
		static void moduleWarn(std::string_view moduleName, std::string_view fmtStr,
		                       Args &&...args) {
			logMessage(LogLevel::Warn,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmtStr, fmt::make_format_args(args...))));
		}

		template <typename... Args>
		static void moduleError(std::string_view moduleName, std::string_view fmtStr,
		                        Args &&...args) {
			logMessage(LogLevel::Error,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmtStr, fmt::make_format_args(args...))));
		}

		template <typename... Args>
		static void moduleCritical(std::string_view moduleName, std::string_view fmtStr,
		                           Args &&...args) {
			logMessage(LogLevel::Critical,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmtStr, fmt::make_format_args(args...))));
		}

	private:
		static void logMessage(LogLevel level, std::string_view message);
	};
} // namespace triple::log