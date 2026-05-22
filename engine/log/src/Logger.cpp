#include "triple/log/Logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace {
    static std::shared_ptr<spdlog::logger> s_logger;
}

namespace triple::log {
    void Logger::LogMessage(LogLevel level, std::string_view message) {
        switch (level) {
        case LogLevel::Info:  spdlog::info(message); break;
        case LogLevel::Warn:  spdlog::warn(message); break;
        case LogLevel::Error: spdlog::error(message); break;
        case LogLevel::Critical: spdlog::critical(message); break;
        }
    }
}