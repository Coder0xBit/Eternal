#pragma once
#define FMT_UNICODE 0 
#include "spdlog/spdlog.h"

#include <memory>

namespace Eternal {

	class Logger {
	public:

		static void Init();

		template<typename... Args>
		static void Warn(fmt::format_string<Args...> format, Args&&... args) {
			if (m_InternalLogger != nullptr) {
				m_InternalLogger->warn(format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		static void Debug(fmt::format_string<Args...> format, Args&&... args) {
			if (m_InternalLogger != nullptr) {
				m_InternalLogger->debug(format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		static void Info(fmt::format_string<Args...> format, Args&&... args) {
			if (m_InternalLogger != nullptr) {
				m_InternalLogger->info(format, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		static void Error(fmt::format_string<Args...>  format, Args&&... args) {
			if (m_InternalLogger != nullptr) {
				m_InternalLogger->error(format, std::forward<Args>(args)...);
			}
		}

	private:
		static std::shared_ptr<spdlog::logger> m_InternalLogger;
	};

}