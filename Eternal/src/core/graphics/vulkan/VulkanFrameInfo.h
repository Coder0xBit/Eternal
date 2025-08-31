#pragma once
#include <core/graphics/FrameInfo.h>

#include <vulkan/vulkan.hpp>


namespace Eternal {
	class VulkanFrameInfo : public FrameInfo {
	public:
		VulkanFrameInfo(vk::CommandBuffer commandBuffer, uint32_t imageIndex) :
			commandBuffer(commandBuffer), imageIndex(imageIndex) {

		}

		bool operator==(const VulkanFrameInfo& other) const noexcept {
			return this->commandBuffer == other.commandBuffer && this->imageIndex == other.imageIndex;
		}

		vk::CommandBuffer commandBuffer;
		uint32_t imageIndex;

	};
}

