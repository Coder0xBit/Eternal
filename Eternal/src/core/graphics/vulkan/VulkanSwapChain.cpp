#include "core/graphics/vulkan/VulkanSwapChain.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Eternal {
    VulkanSwapChain::VulkanSwapChain(
        vk::Instance instance,
        vk::Device logicalDevice,
        vk::PhysicalDevice physicalDevice,
        vk::Queue queue,
        vk::SurfaceKHR surface,
        vk::Extent2D extent,
        uint32_t graphicsQueueFamilyIndex,
        uint32_t presentQueueFamilyIndex
    ) : mVkInstance(instance),
        mLogicalDevice(logicalDevice),
        mPhysicalDevice(physicalDevice),
        mPresentQueue(queue),
        mSurface(surface),
        mFallBackExtent(extent),
        mGraphicsQueueFamilyIndex(graphicsQueueFamilyIndex),
        mPresentQueueFamilyIndex(presentQueueFamilyIndex) {
        create();
    }

    VulkanSwapChain::~VulkanSwapChain() {
        destroy();
    }

    vk::Result VulkanSwapChain::acquire(vk::Semaphore imageReadySemaphore, uint32_t* imageIndex) {
        vk::Result result = mLogicalDevice.acquireNextImageKHR(mSwapChain, std::numeric_limits<uint64_t>::max(),
                                                                imageReadySemaphore, nullptr, imageIndex);
        mShouldRecreate = result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR;
        return result;
    }

    vk::Result VulkanSwapChain::present(vk::Semaphore renderFinishedSemaphore, uint32_t imageIndex) {
        vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&renderFinishedSemaphore)
                .setSwapchainCount(1)
                .setPSwapchains(&mSwapChain)
                .setPImageIndices(&imageIndex);

        vk::Result result = mPresentQueue.presentKHR(&presentInfo);
        mShouldRecreate = result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR;
        return result;
    }

    void VulkanSwapChain::recreate() {
        mShouldRecreate = false;
        destroy();
        create();
    }

    void VulkanSwapChain::create() {
        vk::SurfaceCapabilitiesKHR capabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);

        vk::SurfaceFormatKHR format = selectSwapChainSurfaceFormat();

        vk::PresentModeKHR presentMode = selectSwapChainPresentMode();
        Eternal::Logger::Debug("Selected Present Mode {}", vk::to_string(presentMode));

        vk::Extent2D extent = selectSwapChainExtent(capabilities);
        Eternal::Logger::Info("Selected Extent: {}x{}", extent.width, extent.height);

        mSwapChainDetails.capabilities = capabilities;
        mSwapChainDetails.surfaceFormat = format;
        mSwapChainDetails.presentMode = presentMode;
        mSwapChainDetails.extent = extent;

        uint32_t swapChainImageCount = mSwapChainDetails.capabilities.minImageCount;
        if (mSwapChainDetails.capabilities.maxImageCount > 0 && swapChainImageCount > mSwapChainDetails.capabilities.
            maxImageCount) {
            swapChainImageCount = mSwapChainDetails.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR swapChainCreateInfo = vk::SwapchainCreateInfoKHR()
                .setSurface(mSurface)
                .setMinImageCount(swapChainImageCount)
                .setImageFormat(mSwapChainDetails.surfaceFormat.format)
                .setImageColorSpace(mSwapChainDetails.surfaceFormat.colorSpace)
                .setImageExtent(mSwapChainDetails.extent)
                .setImageArrayLayers(1)
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        std::vector<uint32_t> queueFamilyIndices = {
            mGraphicsQueueFamilyIndex,
            mPresentQueueFamilyIndex
        };

        if (mGraphicsQueueFamilyIndex != mPresentQueueFamilyIndex) {
            swapChainCreateInfo
                    .setImageSharingMode(vk::SharingMode::eConcurrent)
                    .setQueueFamilyIndexCount(2)
                    .setQueueFamilyIndices(queueFamilyIndices);
        } else {
            swapChainCreateInfo
                    .setImageSharingMode(vk::SharingMode::eExclusive);
        }

        swapChainCreateInfo
                .setPreTransform(mSwapChainDetails.capabilities.currentTransform)
                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                .setPresentMode(mSwapChainDetails.presentMode)
                .setClipped(VK_TRUE)
                .setOldSwapchain(VK_NULL_HANDLE);

        mSwapChain = mLogicalDevice.createSwapchainKHR(swapChainCreateInfo);

        createImageViews();
        // createDepthImageView();
        // createRenderPass();
        // createFrameBuffers();
    }

    void VulkanSwapChain::destroy() {
        mShouldRecreate = false;

        mLogicalDevice.waitIdle();

        for (auto imageView: mSwapChainImageViews) {
            mLogicalDevice.destroyImageView(imageView);
        }
        mSwapChainImages.clear();

        // for (auto frameBuffer: mSwapChainFrameBuffers) {
        //     mLogicalDevice.destroyFramebuffer(frameBuffer);
        // }
        // mSwapChainFrameBuffers.clear();
        //
        // if (mDepthImageView) {
        //     mLogicalDevice.destroyImageView(mDepthImageView);
        //     mDepthImageView = nullptr;
        // }
        //
        // if (mDepthImage) {
        //     mLogicalDevice.destroyImage(mDepthImage);
        //     mDepthImage = nullptr;
        // }
        //
        // if (mDepthImageMemory) {
        //     mLogicalDevice.freeMemory(mDepthImageMemory);
        //     mDepthImageMemory = nullptr;
        // }
        //
        // if (mRenderPass) {
        //     mLogicalDevice.destroyRenderPass(mRenderPass);
        //     mRenderPass = nullptr;
        // }

        if (mSwapChain) {
            mLogicalDevice.destroySwapchainKHR(mSwapChain);
            mSwapChain = nullptr;
        }
    }

    vk::SurfaceFormatKHR VulkanSwapChain::selectSwapChainSurfaceFormat() {
        std::vector<vk::SurfaceFormatKHR> availableFormats = mPhysicalDevice.getSurfaceFormatsKHR(mSurface);
        for (const vk::SurfaceFormatKHR& availableFormat: availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace ==
                vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR VulkanSwapChain::selectSwapChainPresentMode() {
        std::vector<vk::PresentModeKHR> availablePresentModes = mPhysicalDevice.getSurfacePresentModesKHR(mSurface);
        for (const auto& availablePresentMode: availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D VulkanSwapChain::selectSwapChainExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            return mFallBackExtent;
        }
    }

    void VulkanSwapChain::createImageViews() {
        mSwapChainImages = mLogicalDevice.getSwapchainImagesKHR(mSwapChain);

        mSwapChainImageViews.resize(mSwapChainImages.size());

        for (uint32_t i = 0; i < mSwapChainImages.size(); i++) {
            vk::ImageSubresourceRange imageSubResourceRange = vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

            vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
                    .setImage(mSwapChainImages[i])
                    .setViewType(vk::ImageViewType::e2D)
                    .setFormat(mSwapChainDetails.surfaceFormat.format)
                    .setComponents(vk::ComponentMapping())
                    .setSubresourceRange(imageSubResourceRange);

            mSwapChainImageViews[i] = mLogicalDevice.createImageView(imageViewCreateInfo);
        }
    }
}
