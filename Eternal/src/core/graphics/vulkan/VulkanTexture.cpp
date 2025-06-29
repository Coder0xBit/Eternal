#include "VulkanTexture.h"
#include "VulkanPlatform.h"

namespace Eternal {

	VulkanTexture::VulkanTexture(vk::Device device, vk::PhysicalDevice physicalDevice, const Eternal::Image* imageResource) {
		m_ImageResource = imageResource;
		m_Device = device;
		m_PhysicalDevice = physicalDevice;

		m_Width = imageResource->getWidth();
		m_Height = imageResource->getHeight();
		create();
	}

	vk::BufferImageCopy VulkanTexture::getRegionForCopy() {
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
			.setImageOffset({ 0,0,0 })
			.setImageExtent({ (uint32_t)m_Width , (uint32_t)m_Height , 1 });

		return region;
	}

	void VulkanTexture::create() {
		int pixelCount = m_ImageResource->getPixelCount();
		int channel = m_ImageResource->getChannels();

		vk::MemoryPropertyFlags bufferProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		m_StagingBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
		m_StagingBuffer->create(pixelCount, channel, vk::BufferUsageFlagBits::eTransferSrc);
		m_StagingBuffer->allocate(bufferProperties);
		m_StagingBuffer->map();
		m_StagingBuffer->write(m_ImageResource->getData());
		m_StagingBuffer->unMap();

		vk::ImageCreateInfo imageCreateInfo;
		imageCreateInfo.imageType = vk::ImageType::e2D;
		imageCreateInfo.extent.width = m_Width;
		imageCreateInfo.extent.height = m_Height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
		imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
		imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
		imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

		m_Image = m_Device.createImage(imageCreateInfo);

		vk::MemoryRequirements memoryRequirements = m_Device.getImageMemoryRequirements(m_Image);

		vk::MemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VulkanPlatform::getMemoryType(m_PhysicalDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryRequirements.memoryTypeBits);

		m_Memory = m_Device.allocateMemory(memoryAllocateInfo);
		m_Device.bindImageMemory(m_Image, m_Memory, 0);
	}

	VulkanTexture::LayoutTransitionInfo VulkanTexture::getLayoutTransitionInfo(vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
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
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(m_Image)
			.setSubresourceRange(subresourceRange);

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.setSrcAccessMask(vk::AccessFlagBits::eNone)
				.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
				.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else {
			ETERNAL_ASSERT(false, "We do not support more layout transitions");
		}

		return { barrier , sourceStage , destinationStage };
	}
}