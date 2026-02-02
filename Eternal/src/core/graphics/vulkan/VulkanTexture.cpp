#include "core/graphics/vulkan/VulkanTexture.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Eternal {

	VulkanTexture::VulkanTexture(vk::Device device, vk::PhysicalDevice physicalDevice, const Eternal::Image* imageResource) {
		m_ImageResource = imageResource;
		m_Device = device;
		m_PhysicalDevice = physicalDevice;

		m_Width = imageResource->getWidth();
		m_Height = imageResource->getHeight();

		create();
	}

	VulkanTexture::~VulkanTexture() {
		if (m_ImageView) {
			m_Device.destroyImageView(m_ImageView);
		}
		if (m_Image) {
			m_Device.destroyImage(m_Image);
		}
		if (m_Memory) {
			m_Device.freeMemory(m_Memory);
		}
		if (m_Sampler) {
			m_Device.destroySampler(m_Sampler);
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
			.setImageOffset({ 0,0,0 })
			.setImageExtent({ (uint32_t)m_Width , (uint32_t)m_Height , 1 });

		return region;
	}

	void VulkanTexture::prepareStagingBuffer() {
		int pixelCount = m_ImageResource->getPixelCount();
		int channel = m_ImageResource->getChannels();

		m_StagingBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
		m_StagingBuffer->create(pixelCount, channel, vk::BufferUsageFlagBits::eTransferSrc);
		m_StagingBuffer->allocate(m_BufferProperties);
		m_StagingBuffer->map();
		m_StagingBuffer->write(m_ImageResource->getData());
		m_StagingBuffer->unMap();
	}

	void VulkanTexture::createImage() {
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
	}

	void VulkanTexture::allocateMemory() {
		vk::MemoryRequirements memoryRequirements = m_Device.getImageMemoryRequirements(m_Image);

		vk::MemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VulkanPlatform::getMemoryType(m_PhysicalDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryRequirements.memoryTypeBits);

		m_Memory = m_Device.allocateMemory(memoryAllocateInfo);
		m_Device.bindImageMemory(m_Image, m_Memory, 0);
	}

	void VulkanTexture::createImageView() {
		vk::ImageSubresourceRange imageSubResourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);

		vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setImage(m_Image)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(m_Format)
			.setComponents(vk::ComponentMapping())
			.setSubresourceRange(imageSubResourceRange);

		m_ImageView = m_Device.createImageView(imageViewCreateInfo);
	}

	void VulkanTexture::createSampler() {
		vk::PhysicalDeviceProperties props = m_PhysicalDevice.getProperties();
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

		m_Sampler = m_Device.createSampler(samplerInfo);
	}

	void VulkanTexture::create() {
		prepareStagingBuffer();
		createImage();
		allocateMemory();
		createImageView();
		createSampler();
	}

	void VulkanTexture::recordUploadCommand(vk::CommandBuffer commandBuffer) const {
		{
			VulkanTexture::LayoutTransitionInfo layoutTransitionInfo =
				getLayoutTransitionInfo(m_Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
			commandBuffer.pipelineBarrier(layoutTransitionInfo.sourceStage, layoutTransitionInfo.destinationStage, {}, {}, {}, { layoutTransitionInfo.imageMemoryBarrier });
		}

		{
			vk::BufferImageCopy region = getRegionForCopy();
			auto stagingBuffer = m_StagingBuffer->getVkBuffer();
			commandBuffer.copyBufferToImage(*stagingBuffer, m_Image, vk::ImageLayout::eTransferDstOptimal, region);
		}

		{
			VulkanTexture::LayoutTransitionInfo layoutTransitionInfo =
				getLayoutTransitionInfo(m_Format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
			commandBuffer.pipelineBarrier(layoutTransitionInfo.sourceStage, layoutTransitionInfo.destinationStage, {}, {}, {}, { layoutTransitionInfo.imageMemoryBarrier });
		}
	}

	VulkanTexture::LayoutTransitionInfo VulkanTexture::getLayoutTransitionInfo(vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const {
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