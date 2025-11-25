#pragma once

#include "utils/Base.h"
#include "core/log/Logger.h"

#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

constexpr uint32_t const MAX_FRAMES_IN_FLIGHT = 2;
