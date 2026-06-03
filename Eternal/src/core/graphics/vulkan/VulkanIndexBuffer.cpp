#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Eternal {
    VulkanIndexBuffer::VulkanIndexBuffer(VulkanPlatform* vulkanPlatform, const std::vector<uint32_t>& indices)
        : mVulkanPlatform(vulkanPlatform) {
        mBuffer = std::make_unique<VulkanBuffer>(mVulkanPlatform);
        mBuffer->create(indices.size(), sizeof(uint32_t),
                         vk::BufferUsageFlagBits::eIndexBuffer);
        mBuffer->allocate(mBufferProperties);
        mBuffer->map();
        mBuffer->write((void*) (indices.data()));
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
    }

    void VulkanIndexBuffer::bind() {
    }

    void VulkanIndexBuffer::unBind() {
    }
}
