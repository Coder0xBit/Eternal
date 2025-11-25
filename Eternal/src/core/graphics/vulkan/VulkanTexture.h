#pragma once

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/resource/Image.h"

namespace Eternal {
    class VulkanTexture {
    public:
        struct LayoutTransitionInfo {
            vk::ImageMemoryBarrier imageMemoryBarrier;
            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;
        };

        VulkanTexture(vk::Device device, vk::PhysicalDevice physicalDevice, const Eternal::Image* imageResource);
        ~VulkanTexture();

        std::shared_ptr<VulkanBuffer> getStagingBuffer() { return m_StagingBuffer; }
        vk::Format getFormat() const { return m_Format; }
        const vk::Image& getImage() const { return m_Image; }
        int getWidth() const { return m_Width; }
        int getHeight() const { return m_Height; }
        vk::BufferImageCopy getRegionForCopy() const;
        vk::ImageView getImageView() const { return m_ImageView; }
        vk::Sampler getSampler() const { return m_Sampler; }

        void create();
        void recordUploadCommand(vk::CommandBuffer commandBuffer) const;
        LayoutTransitionInfo getLayoutTransitionInfo(vk::Format format, vk::ImageLayout oldLayout,
                                                     vk::ImageLayout newLayout) const;

    private:
        const Eternal::Image* m_ImageResource;
        std::shared_ptr<VulkanBuffer> m_StagingBuffer;

        vk::MemoryPropertyFlags m_BufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        void prepareStagingBuffer();
        void createImage();
        void allocateMemory();
        void createImageView();
        void createSampler();

        vk::Device m_Device;
        vk::PhysicalDevice m_PhysicalDevice;

        vk::Image m_Image;
        vk::DeviceSize m_ImageSize = 0;
        vk::DeviceMemory m_Memory;
        vk::ImageView m_ImageView;
        vk::Sampler m_Sampler;
        vk::DescriptorImageInfo m_DescriptorInfo;

        int m_Width = 0;
        int m_Height = 0;
        vk::Format m_Format = vk::Format::eR8G8B8A8Srgb;

        std::string m_DebugName = "test_texture_name";
    };
}
