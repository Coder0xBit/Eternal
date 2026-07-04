#include "core/graphics/vulkan/VulkanTexture.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Vortak {
    VulkanTexture::VulkanTexture(Vortak::VulkanPlatform* vulkanPlatform, const Vortak::Image* imageResource) {
        mVulkanPlatform = vulkanPlatform;
        mImageResource = imageResource;
        mDevice = vulkanPlatform->getLogicalDevice();
        mPhysicalDevice = vulkanPlatform->getPhysicalDevice();

        mWidth = imageResource->getWidth();
        mHeight = imageResource->getHeight();

        create();
    }

    VulkanTexture::~VulkanTexture() {
        if (mImageView) {
            mDevice.destroyImageView(mImageView);
        }
        if (mImage) {
            mDevice.destroyImage(mImage);
        }
        if (mMemory) {
            mDevice.freeMemory(mMemory);
        }
        if (mSampler) {
            mDevice.destroySampler(mSampler);
        }
    }

    vk::BufferImageCopy VulkanTexture::getRegionForCopy() const {
        vk::ImageSubresourceLayers subresourceRange = vk::ImageSubresourceLayers()
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setMipLevel(0)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        vk::BufferImageCopy region = vk::BufferImageCopy()
                .setBufferOffset(0)
                .setBufferRowLength(0)
                .setBufferImageHeight(0)
                .setImageSubresource(subresourceRange)
                .setImageOffset({0, 0, 0})
                .setImageExtent({(uint32_t) mWidth, (uint32_t) mHeight, 1});

        return region;
    }

    void VulkanTexture::prepareStagingBuffer() {
        int pixelCount = mImageResource->getPixelCount();
        int channel = mImageResource->getChannels();

        mStagingBuffer = std::make_shared<VulkanBuffer>(mVulkanPlatform);
        mStagingBuffer->create(pixelCount, channel, vk::BufferUsageFlagBits::eTransferSrc);
        mStagingBuffer->allocate(mBufferProperties);
        mStagingBuffer->map();
        mStagingBuffer->write(mImageResource->getData());
        mStagingBuffer->unMap();
    }

    void VulkanTexture::createImage() {
        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.extent.width = mWidth;
        imageCreateInfo.extent.height = mHeight;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
        imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
        imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        mImage = mDevice.createImage(imageCreateInfo);
    }

    void VulkanTexture::allocateMemory() {
        vk::MemoryRequirements memoryRequirements = mDevice.getImageMemoryRequirements(mImage);

        vk::MemoryAllocateInfo memoryAllocateInfo;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = VulkanPlatform::getMemoryType(
            mPhysicalDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryRequirements.memoryTypeBits);

        mMemory = mDevice.allocateMemory(memoryAllocateInfo);
        mDevice.bindImageMemory(mImage, mMemory, 0);
    }

    void VulkanTexture::createImageView() {
        vk::ImageSubresourceRange imageSubResourceRange = vk::ImageSubresourceRange()
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
                .setImage(mImage)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(mFormat)
                .setComponents(vk::ComponentMapping())
                .setSubresourceRange(imageSubResourceRange);

        mImageView = mDevice.createImageView(imageViewCreateInfo);
    }

    void VulkanTexture::createSampler() {
        vk::PhysicalDeviceProperties props = mPhysicalDevice.getProperties();
        vk::SamplerCreateInfo samplerInfo = vk::SamplerCreateInfo()
                .setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setAnisotropyEnable(VK_TRUE)
                .setMaxAnisotropy(props.limits.maxSamplerAnisotropy)
                .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                .setUnnormalizedCoordinates(VK_FALSE)
                .setCompareEnable(VK_FALSE)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear);

        mSampler = mDevice.createSampler(samplerInfo);
    }

    void VulkanTexture::create() {
        prepareStagingBuffer();
        createImage();
        allocateMemory();
        createImageView();
        createSampler();
    }

    void VulkanTexture::recordUploadCommand(vk::CommandBuffer commandBuffer) const { {
            VulkanTexture::LayoutTransitionInfo layoutTransitionInfo =
                    getLayoutTransitionInfo(mFormat, vk::ImageLayout::eUndefined,
                                            vk::ImageLayout::eTransferDstOptimal);
            commandBuffer.pipelineBarrier(layoutTransitionInfo.sourceStage, layoutTransitionInfo.destinationStage, {},
                                          {}, {}, {layoutTransitionInfo.imageMemoryBarrier});
        } {
            vk::BufferImageCopy region = getRegionForCopy();
            auto stagingBuffer = mStagingBuffer->getVkBuffer();
            commandBuffer.copyBufferToImage(*stagingBuffer, mImage, vk::ImageLayout::eTransferDstOptimal, region);
        } {
            VulkanTexture::LayoutTransitionInfo layoutTransitionInfo =
                    getLayoutTransitionInfo(mFormat, vk::ImageLayout::eTransferDstOptimal,
                                            vk::ImageLayout::eShaderReadOnlyOptimal);
            commandBuffer.pipelineBarrier(layoutTransitionInfo.sourceStage, layoutTransitionInfo.destinationStage, {},
                                          {}, {}, {layoutTransitionInfo.imageMemoryBarrier});
        }
    }

    VulkanTexture::LayoutTransitionInfo VulkanTexture::getLayoutTransitionInfo(
        vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const {
        vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
                .setOldLayout(oldLayout)
                .setNewLayout(newLayout)
                .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                .setImage(mImage)
                .setSubresourceRange(subresourceRange);

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.setSrcAccessMask(vk::AccessFlagBits::eNone)
                    .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout ==
                   vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else {
            VORTAK_ASSERT(false, "We do not support more layout transitions");
        }

        return {barrier, sourceStage, destinationStage};
    }
}
