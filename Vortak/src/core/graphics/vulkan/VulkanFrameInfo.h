#pragma once

#include <vulkan/vulkan.hpp>

#include "core/graphics/FrameInfo.h"

namespace Vortak {
    class VulkanFrameInfo : public FrameInfo {
    public :
        VulkanFrameInfo() = default;

        VulkanFrameInfo(vk::CommandBuffer cmdBuffer, uint32_t imgIndex) {
            this->imageIndex = imgIndex;
            this->commandBuffer = cmdBuffer;
        }

        ~VulkanFrameInfo() override = default;

        vk::CommandBuffer commandBuffer;
        uint32_t imageIndex = INVALID_VK_INDEX;
    };
}
