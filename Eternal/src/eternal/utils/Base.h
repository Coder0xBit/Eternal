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
}


