#pragma once

#include <utils/Base.h>
#include <core/resource/Image.h>

#include "VulkanBuffer.h"

namespace Eternal {
	class VulkanTexture {
		VulkanTexture(vk::Device device, vk::PhysicalDevice physicalDevice, const Eternal::Image* imageResource);
		~VulkanTexture() = default;

		void create();

	private:

		const Eternal::Image* m_ImageResource;
		std::shared_ptr<VulkanBuffer> m_Buffer;

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