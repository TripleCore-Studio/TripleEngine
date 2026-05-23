#pragma once

#include <string>
#include <fmt/format.h>

namespace triple::log {
	enum class LogLevel { Debug, Info, Warn, Error, Critical };

	class Logger {
	  public:
		template <typename... Args>
		static void Info(std::string_view fmt_str, Args &&...args) {
			LogMessage(LogLevel::Info, fmt::vformat(fmt_str, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void Warn(std::string_view fmt_str, Args &&...args) {
			LogMessage(LogLevel::Warn, fmt::vformat(fmt_str, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void Error(std::string_view fmt_str, Args &&...args) {
			LogMessage(LogLevel::Error, fmt::vformat(fmt_str, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void Critical(std::string_view fmt_str, Args &&...args) {
			LogMessage(LogLevel::Critical, fmt::vformat(fmt_str, fmt::make_format_args(args...)));
		}

		template <typename... Args>
		static void ModuleInfo(std::string_view moduleName, std::string_view fmt_str,
		                       Args &&...args) {
			LogMessage(LogLevel::Info,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmt_str, fmt::make_format_args(args...))));
		}

		template <typename... Args>
		static void ModuleWarn(std::string_view moduleName, std::string_view fmt_str,
		                       Args &&...args) {
			LogMessage(LogLevel::Warn,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmt_str, fmt::make_format_args(args...))));
		}

		template <typename... Args>
		static void ModuleError(std::string_view moduleName, std::string_view fmt_str,
		                        Args &&...args) {
			LogMessage(LogLevel::Error,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmt_str, fmt::make_format_args(args...))));
		}

		template <typename... Args>
		static void ModuleCritical(std::string_view moduleName, std::string_view fmt_str,
		                           Args &&...args) {
			LogMessage(LogLevel::Critical,
			           fmt::format("[{}] -> {}", moduleName,
			                       fmt::vformat(fmt_str, fmt::make_format_args(args...))));
		}

	  private:
		static void LogMessage(LogLevel level, std::string_view message);
	};
} // namespace triple::log