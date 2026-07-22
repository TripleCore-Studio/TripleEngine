#include "triple/log/Logger.h"
#include <spdlog/spdlog.h>

namespace triple::log {
	void Logger::logMessage(LogLevel level, std::string_view message) {
		switch (level) {
			case LogLevel::Info:
				spdlog::info(message);
				break;
			case LogLevel::Warn:
				spdlog::warn(message);
				break;
			case LogLevel::Error:
				spdlog::error(message);
				break;
			case LogLevel::Critical:
				spdlog::critical(message);
				break;
		}
	}
} // namespace triple::log