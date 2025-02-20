#include "Logger.h"

#include <utility>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace Eternal {
	std::shared_ptr<spdlog::logger> Logger::m_InternalLogger = nullptr;

	void Logger::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

		logSinks[0]->set_pattern("%^Eternal::%l [%T] %v%$");

		m_InternalLogger = std::make_shared<spdlog::logger>("Eternal", begin(logSinks), end(logSinks));
		spdlog::register_logger(m_InternalLogger);
		m_InternalLogger->set_level(spdlog::level::trace);
		m_InternalLogger->flush_on(spdlog::level::trace);
	}
}