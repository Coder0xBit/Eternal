#pragma once
#include <core/graphics/FrameInfo.h>

#include <vulkan/vulkan.hpp>


namespace Eternal {
    class VulkanFrameInfo : public FrameInfo {
    public :
        VulkanFrameInfo() = default;

        VulkanFrameInfo(vk::CommandBuffer cmdBuffer, uint32_t imgIndex, Eternal::Camera* cam) {
            this->camera = cam;
            this->imageIndex = imgIndex;
            this->commandBuffer = cmdBuffer;
        }

        ~VulkanFrameInfo() override = default;

        vk::CommandBuffer commandBuffer;
        uint32_t imageIndex = INVALID_VK_INDEX;
    };
}
