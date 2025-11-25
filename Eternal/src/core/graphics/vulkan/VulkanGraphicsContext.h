#pragma once

#include "core/graphics/GraphicsContext.h"
#include "core/graphics/vulkan/VulkanSwapChain.h"

#include <vulkan/vulkan.hpp>

namespace Eternal {
    class VulkanGraphicsContext : public GraphicsContext {
    public:
        VulkanGraphicsContext() = default;
        ~VulkanGraphicsContext() = default;
        vk::Instance getVkInstance() { return m_VkInstance; }
        vk::PhysicalDevice getPhysicalDevice() { return m_PhysicalDevice; }
        vk::Device getLogicalDevice() { return m_LogicalDevice; }
        vk::Queue getGraphicsQueue() { return m_GraphicsQueue; }
        uint32_t getGraphicsQueueFamilyIndex() { return m_GraphicsQueueFamilyIndex; }
        uint32_t getGraphicsQueueIndex() { return m_GraphicsQueueIndex; }
        vk::Queue getPresentQueue() { return m_PresentQueue; }
        uint32_t getPresentQueueFamilyIndex() { return m_PresentQueueFamilyIndex; }
        uint32_t getPresentQueueIndex() { return m_PresentQueueIndex; }

    protected:
        vk::Instance m_VkInstance = nullptr;
        vk::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::Device m_LogicalDevice = nullptr;
        vk::SurfaceKHR m_Surface = nullptr;
        vk::Queue m_GraphicsQueue = nullptr;
        uint32_t m_GraphicsQueueFamilyIndex = INVALID_VK_INDEX;
        uint32_t m_GraphicsQueueIndex = 0;
        vk::Queue m_PresentQueue = nullptr;
        uint32_t m_PresentQueueFamilyIndex = INVALID_VK_INDEX;
        uint32_t m_PresentQueueIndex = 0;
    };
}
