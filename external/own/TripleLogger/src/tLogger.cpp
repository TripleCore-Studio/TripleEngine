#include "TLogger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace {
    static std::shared_ptr<spdlog::logger> logger;
}

namespace TripleLogger {
    void TLogger::LogMessage(LogLevel level, const std::string& message) {
        switch (level) {
        case LogLevel::Info:  spdlog::info(message); break;
        case LogLevel::Warn:  spdlog::warn(message); break;
        case LogLevel::Error: spdlog::error(message); break;
        case LogLevel::Critical: spdlog::critical(message); break;
        }
    }
}