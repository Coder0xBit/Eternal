#pragma once

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/resource/Image.h"

namespace Vortak {
    class VulkanTexture {
    public:
        struct LayoutTransitionInfo {
            vk::ImageMemoryBarrier imageMemoryBarrier;
            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;
        };

        VulkanTexture(Vortak::VulkanDevice* vulkanPlatform, const Vortak::Image* imageResource);
        ~VulkanTexture();

        std::shared_ptr<VulkanBuffer> getStagingBuffer() { return mStagingBuffer; }
        vk::Format getFormat() const { return mFormat; }
        const vk::Image& getImage() const { return mImage; }
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        vk::BufferImageCopy getRegionForCopy() const;
        vk::ImageView getImageView() const { return mImageView; }
        vk::Sampler getSampler() const { return mSampler; }

        void create();
        void recordUploadCommand(vk::CommandBuffer commandBuffer) const;
        LayoutTransitionInfo getLayoutTransitionInfo(vk::Format format, vk::ImageLayout oldLayout,
                                                     vk::ImageLayout newLayout) const;

    private:
        const Vortak::Image* mImageResource;
        std::shared_ptr<VulkanBuffer> mStagingBuffer;

        vk::MemoryPropertyFlags mBufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        void prepareStagingBuffer();
        void createImage();
        void allocateMemory();
        void createImageView();
        void createSampler();

        vk::Device mDevice;
        vk::PhysicalDevice mPhysicalDevice;

        Vortak::VulkanDevice* mVulkanPlatform;

        vk::Image mImage;
        vk::DeviceSize mImageSize = 0;
        vk::DeviceMemory mMemory;
        vk::ImageView mImageView;
        vk::Sampler mSampler;
        vk::DescriptorImageInfo mDescriptorInfo;

        int mWidth = 0;
        int mHeight = 0;
        vk::Format mFormat = vk::Format::eR8G8B8A8Srgb;

        std::string mDebugName = "test_texture_name";
    };
}
