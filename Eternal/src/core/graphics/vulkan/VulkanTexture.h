#pragma once

#include <utils/Base.h>

#include "VulkanBuffer.h"

namespace Eternal {
	class VulkanTexture {
		VulkanTexture() = default;
		~VulkanTexture() = default;

		void create();

	private:
		vk::Image m_Image;
		vk::DeviceMemory m_Memory;
		vk::ImageView m_ImageView;
		vk::Sampler m_Sampler;
		vk::DescriptorImageInfo m_DescriptorInfo;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		vk::Format m_Format = vk::Format::eR8G8B8A8Srgb;

		std::string m_DebugName = "test_texture_name";
	};
}