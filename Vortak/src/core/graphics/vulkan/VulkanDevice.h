#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "utils/Base.h"
#include "core/graphics/GraphicsDevice.h"
#include "core/Camera.h"
#include "core/window/Window.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Vortak {
    using VkStringArray = std::vector<const char*>;
    using VkString = const char*;
    using VkStringArrayPtr = const char**;

    class VulkanDevice : public GraphicsDevice {
    public:
        struct PushConstants {
            glm::mat4 transform{1.f};
        };

        VulkanDevice(const Builder& builder);

        ~VulkanDevice() override;

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

        bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray& requestedExtensions);

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

        vk::Instance getVkInstance() const { return mVkInstance; }

        vk::PhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }

        vk::Device getLogicalDevice() const { return mLogicalDevice; }

        vk::Queue getGraphicsQueue() const { return mGraphicsQueue; }

        uint32_t getGraphicsQueueFamilyIndex() const { return mGraphicsQueueFamilyIndex; }

        uint32_t getGraphicsQueueIndex() const { return mGraphicsQueueIndex; }

        vk::Queue getPresentQueue() const { return mPresentQueue; }

        uint32_t getPresentQueueFamilyIndex() const { return mPresentQueueFamilyIndex; }

        uint32_t getPresentQueueIndex() const { return mPresentQueueIndex; }

    private:
        std::string mApplicationName;
        uint32_t mVersion = 0;

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
