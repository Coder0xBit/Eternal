#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Vortak {
    VulkanIndexBuffer::VulkanIndexBuffer(GraphicsPlatform* graphicsPlatform) {
        mVulkanPlatform = static_cast<VulkanPlatform*>(graphicsPlatform);
        mBuffer = std::make_unique<VulkanBuffer>(mVulkanPlatform);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
    }

    void VulkanIndexBuffer::bind() {
    }

    void VulkanIndexBuffer::unBind() {
    }

    void VulkanIndexBuffer::setBuffer(const std::vector<uint32_t>& indices) {
        mBuffer->create(indices.size(), sizeof(uint32_t),
                        vk::BufferUsageFlagBits::eIndexBuffer);
        mBuffer->allocate(mBufferProperties);
        mBuffer->map();
        mBuffer->write((void*) (indices.data()));
    }
}
