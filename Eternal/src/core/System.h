#pragma once

namespace Eternal {
	enum class System
	{
		WINDOWS,
		IOS,
		MACOS,
		LINUX,
		ANDROID,
		NONE
	};

	static System detectSystem() {
#if defined(_WIN32) || defined(_WIN64)
		return System::WINDOWS; // Windows (32-bit or 64-bit)
#elif defined(__ANDROID__)
		return Platform::ANDROID; // Android
#elif defined(__linux__) && !defined(__ANDROID__)
		return Platform::LINUX;   // Linux (non-Android)
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
		return Platform::IOS;  // IOS
#else
		return Platform::MACOS; // Mac Os
#endif
#elif defined(__unix__)
		return Platform::LINUX;   // Generic Unix
#else
		return Platform::NONE; // Unknown platform
#endif
	}
}