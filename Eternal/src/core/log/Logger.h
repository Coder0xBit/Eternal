#pragma once

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>
#include <memory>

namespace Eternal {
    enum class LogLevel {
        Trace,
        Warn,
        Error,
        Info,
        Debug,
        Critical
    };

    class Logger {
    public:
        static void Init();

        template<typename... Args>
        static void Warn(fmt::format_string<Args...> format, Args&&... args) {
            if (s_Logger != nullptr) {
                s_Logger->warn(fmt::runtime(format), std::forward<Args>(args)...);
            }
        }

        template<typename... Args>
        static void Debug(fmt::format_string<Args...> format, Args&&... args) {
            if (s_Logger != nullptr) {
                s_Logger->debug(fmt::runtime(format), std::forward<Args>(args)...);
            }
        }

        template<typename... Args>
        static void Info(fmt::format_string<Args...> format, Args&&... args) {
            if (s_Logger != nullptr) {
                s_Logger->info(fmt::runtime(format), std::forward<Args>(args)...);
            }
        }

        template<typename... Args>
        static void Error(fmt::format_string<Args...> format, Args&&... args) {
            if (s_Logger != nullptr) {
                s_Logger->error(fmt::runtime(format), std::forward<Args>(args)...);
            }
        }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
    };
}
