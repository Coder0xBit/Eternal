#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <initializer_list>
#include <array>
#include <string_view>
#include <cassert>
#include <typeindex>
#include <future>
#include <mutex>

#include <core/Logger.h>
#include <utils/PrivateImplementationImpl.h>

#define ENABLE_LOGS 1
#define ETERNAL_FLAG_ENABLED(flags) ((flags & 1) == 1)

#define ETERNAL_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            Eternal::Logger::Error("Assertion failed: {}. File: {}, Line: {}", message, __FILE__, __LINE__); \
            throw std::runtime_error(message); \
        } \
    } while (0)

#define ETERNAL_ASSERT_LOG(condition, message) \
    do { \
        if (!(condition)) { \
            Eternal::Logger::Error("Assertion failed: {}. File: {}, Line: {}", message, __FILE__, __LINE__); \
        } \
    } while (0)

#define ETERNAL_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


namespace Eternal {
	constexpr uint32_t const INVALID_VK_INDEX = 0xFFFFFFFF;

	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	namespace Memory {
		template<typename T, typename... Args>
		inline T* Allocate(Args&&... args) {
			return new T(std::forward<Args>(args)...);
		}

		template<typename T>
		inline void Deallocate(T* ptr) {
			if (ptr) {
				delete ptr;
			}
		}
	}
}


