#pragma once

#include "utils/Base.h"
#include "core/graphics/SwapChain.h"

#include <vulkan/vulkan.hpp>

namespace Eternal {
    class VulkanSwapChain : public SwapChain {
    public:
        struct SwapChainDetails {
            vk::SurfaceCapabilitiesKHR capabilities;
            vk::SurfaceFormatKHR surfaceFormat;
            vk::PresentModeKHR presentMode;
            vk::Extent2D extent;
        };

        VulkanSwapChain(
            vk::Instance instance,
            vk::Device logicalDevice,
            vk::PhysicalDevice physicalDevice,
            vk::Queue queue,
            vk::SurfaceKHR surface,
            vk::Extent2D extent,
            uint32_t graphicsQueueFamilyIndex,
            uint32_t presentQueueFamilyIndex
        );

        ~VulkanSwapChain();

        vk::Result acquire(vk::Semaphore imageReadySemaphore, uint32_t* imageIndex);
        vk::Result present(vk::Semaphore renderFinishedSemaphore, uint32_t imageIndex);
        void recreate();
        const SwapChainDetails& getSwapChainDetails() { return mSwapChainDetails; }
        const std::vector<vk::ImageView>& getImageViews() { return mSwapChainImageViews; }
        const std::vector<vk::Image>& getImages() { return mSwapChainImages; }
        vk::SwapchainKHR getSwapChain() { return mSwapChain; }
        vk::SurfaceKHR getSurface() { return mSurface; }
        bool shouldRecreate() const { return mShouldRecreate; }
        void setShouldRecreate(bool shouldRecreate) { mShouldRecreate = shouldRecreate; }
        void destroy();

    private:
        void create();
        vk::SurfaceFormatKHR selectSwapChainSurfaceFormat();
        vk::PresentModeKHR selectSwapChainPresentMode();
        vk::Extent2D selectSwapChainExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
        void createImageViews();

        bool mShouldRecreate = false;
        vk::Instance mVkInstance = nullptr;
        vk::Device mLogicalDevice = nullptr;
        vk::PhysicalDevice mPhysicalDevice = nullptr;
        vk::Queue mPresentQueue = nullptr;
        uint32_t mGraphicsQueueFamilyIndex = INVALID_VK_INDEX;
        uint32_t mPresentQueueFamilyIndex = INVALID_VK_INDEX;
        vk::SurfaceKHR mSurface = nullptr;
        vk::Extent2D mFallBackExtent;
        std::vector<vk::Image> mSwapChainImages;
        std::vector<vk::ImageView> mSwapChainImageViews;

        vk::SwapchainKHR mSwapChain = nullptr;
        SwapChainDetails mSwapChainDetails = {};
    };
}
