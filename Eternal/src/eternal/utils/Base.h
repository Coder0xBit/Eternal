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

#include <eternal/core/Logger.h>
#include <eternal/utils/PrivateImplementationImpl.h>


#define ETERNAL_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            Eternal::Logger::Error("Assertion failed: {}. File: {}, Line: {}", message, __FILE__, __LINE__); \
            throw std::runtime_error(message); \
        } \
    } while (0)

namespace Eternal {
	constexpr uint32_t const INVALID_VK_INDEX = 0xFFFFFFFF;

	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	namespace Memory {
		template<typename T>
		using Ref = std::shared_ptr<T>;

		template<typename T, typename... Args>
		inline Ref<T> CreateRef(Args&&... args) {
			return std::make_shared<T>(std::forward<Args>(args)...);
		}

		template<class T1, class T2>
		inline Ref<T1> DynamicPtrCast(const Ref<T2>& ref) {
			return std::dynamic_pointer_cast<T1>(ref);
		}

		template<class T1, class T2>
		inline Ref<T1> ReinterpretPtrCast(const Ref<T2>& ref) {
			return std::reinterpret_pointer_cast<T1>(ref);
		}

		template<class T1, class T2>
		inline Ref<T1> ConstPtrCast(const Ref<T2>& ref) {
			return std::const_pointer_cast<T1>(ref);
		}

		template<class T1, class T2>
		inline Ref<T1> StaticPtrCast(const Ref<T2>& ref) {
			return std::static_pointer_cast<T1>(ref);
		}

		template<typename T>
		using Scope = std::unique_ptr<T>;

		template<typename T, typename... Args>
		inline Scope<T> CreateScope(Args&&... args) {
			return std::make_unique<T>(std::forward<Args>(args)...);
		}
	}
}


