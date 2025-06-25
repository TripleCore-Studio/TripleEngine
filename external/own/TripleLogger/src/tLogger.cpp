#include "TLogger.h"
#include <spdlog/spdlog.h>

namespace TripleLogger {
	void TLogger::LogError(const std::string& text)
	{
		spdlog::error("{0}", text);
	}
	void TLogger::LogWarning(const std::string& text)
	{
		spdlog::warn("{0}", text);
	}
	void TLogger::LogInfo(const std::string& text)
	{
		spdlog::info("{0}", text);
	}
	void TLogger::LogCritical(const std::string& text)
	{
		spdlog::critical("{0}", text);
	}
}