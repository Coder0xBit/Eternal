#include "core/log/Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Eternal {
    std::shared_ptr<spdlog::logger> Logger::s_Logger = nullptr;

    void Logger::Init() {
        spdlog::set_pattern("[%T] [%^%l%$] %v");

        s_Logger = spdlog::stdout_color_mt("ETERNAL");
        s_Logger->set_level(spdlog::level::trace);
    }


}
