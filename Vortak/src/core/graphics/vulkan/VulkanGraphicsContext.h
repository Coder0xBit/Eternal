#pragma once

#include "core/graphics/GraphicsContext.h"
#include "core/graphics/vulkan/VulkanSwapChain.h"

#include <vulkan/vulkan.hpp>

namespace Vortak {
    class VulkanGraphicsContext : public GraphicsContext {
    public:
        VulkanGraphicsContext() = default;
        ~VulkanGraphicsContext() override = default;
        vk::Instance getVkInstance() const { return mVkInstance; }
        vk::PhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }
        vk::Device getLogicalDevice() const { return mLogicalDevice; }
        vk::Queue getGraphicsQueue() const { return mGraphicsQueue; }
        uint32_t getGraphicsQueueFamilyIndex() const { return mGraphicsQueueFamilyIndex; }
        uint32_t getGraphicsQueueIndex() const { return mGraphicsQueueIndex; }
        vk::Queue getPresentQueue() const { return mPresentQueue; }
        uint32_t getPresentQueueFamilyIndex() const { return mPresentQueueFamilyIndex; }
        uint32_t getPresentQueueIndex() const { return mPresentQueueIndex; }

    protected:
        vk::Instance mVkInstance = nullptr;
        vk::PhysicalDevice mPhysicalDevice = nullptr;
        vk::Device mLogicalDevice = nullptr;
        vk::SurfaceKHR mSurface = nullptr;
        vk::Queue mGraphicsQueue = nullptr;
        uint32_t mGraphicsQueueFamilyIndex = INVALID_VK_INDEX;
        uint32_t mGraphicsQueueIndex = 0;
        vk::Queue mPresentQueue = nullptr;
        uint32_t mPresentQueueFamilyIndex = INVALID_VK_INDEX;
        uint32_t mPresentQueueIndex = 0;
    };
}
