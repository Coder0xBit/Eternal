#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanGraphicsContext.h"
#include "core/graphics/GraphicsPlatform.h"
#include "core/Camera.h"
#include "core/window/Window.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Vortak {
    using VkStringArray = std::vector<const char*>;
    using VkString = const char*;
    using VkStringArrayPtr = const char**;

    class VulkanPlatform : public GraphicsPlatform, public VulkanGraphicsContext {
    public:
        struct PushConstants {
            glm::mat4 transform{1.f};
        };

        VulkanPlatform(const Builder& builder);

        ~VulkanPlatform() override;

        void initialize() override;

        void shutDown() override;

        SwapChain* createSwapChain(Window* window) override;

        vk::Instance createInstance(const std::string& applicationName);

        vk::PhysicalDevice choosePhysicalDevice(const vk::Instance& instance);

        uint32_t identifyGraphicsQueueFamilyIndex(vk::PhysicalDevice& device, vk::QueueFlags flags);

        uint32_t identifyPresentQueueFamilyIndex(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);

        vk::Device createLogicalDevice(vk::PhysicalDevice& device, uint32_t graphicsQueueFamilyIndex,
                                       uint32_t presentQueueFamilyIndex);

        bool validateExtensions(VkStringArray extensions);

        bool validateLayers(VkStringArray layers);

        bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray requestedExtensions);

        bool checkDeviceIsSuitable(const vk::PhysicalDevice& device);

        void logDeviceProps(const vk::PhysicalDevice& device);

        vk::CommandPool createCommandPool(vk::CommandPoolCreateFlags commandPoolCreateFlagBits);

        std::vector<vk::CommandBuffer> allocateCommandBuffers(vk::CommandPool commandPool, vk::CommandBufferLevel level,
                                                              uint32_t count);

        vk::CommandBuffer allocateCommandBuffer(vk::CommandPool commandPool, vk::CommandBufferLevel level);

        void destroyCommandPool(vk::CommandPool commandPool);

        vk::CommandBuffer beginSingleCommand(vk::CommandPool commandPool);

        void endSingleCommand(vk::CommandPool commandPool, vk::CommandBuffer commandBuffer, vk::Queue queue);

        void executeOneCommand(vk::CommandPool commandPool, vk::Queue queue,
                               const std::function<void(vk::CommandBuffer)>& function);

        vk::SurfaceKHR createWindowSurface(Vortak::Window* window) const;

        static vk::Extent2D getExtent(Vortak::Window* window);

        static uint32_t getMemoryType(vk::PhysicalDevice physicalDevice, vk::MemoryPropertyFlags properties,
                                      uint32_t typeBits);

    private:
        std::string mApplicationName;
        uint32_t mVersion = 0;
    };
}
