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

	void VulkanTexture::create() {
		int pixelCount = m_ImageResource->getPixelCount();
		int channel = m_ImageResource->getChannels();

		vk::MemoryPropertyFlags bufferProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		m_Buffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
		m_Buffer->create(pixelCount, channel, vk::BufferUsageFlagBits::eTransferSrc);
		m_Buffer->allocate(bufferProperties);
		m_Buffer->map();
		m_Buffer->write(m_ImageResource->getData());
		m_Buffer->unMap();

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
}