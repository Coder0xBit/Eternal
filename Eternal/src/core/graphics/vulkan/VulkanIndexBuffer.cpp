#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Eternal {
    VulkanIndexBuffer::VulkanIndexBuffer(VulkanPlatform* vulkanPlatform, const std::vector<uint32_t>& indices)
        : m_VulkanPlatform(vulkanPlatform) {
        m_Buffer = std::make_unique<VulkanBuffer>(m_VulkanPlatform);
        m_Buffer->create(indices.size(), sizeof(uint32_t),
                         vk::BufferUsageFlagBits::eVertexBuffer);
        m_Buffer->allocate(m_BufferProperties);
        m_Buffer->map();
        m_Buffer->write((void*) (indices.data()));
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
    }

    void VulkanIndexBuffer::bind() {
    }

    void VulkanIndexBuffer::unBind() {
    }
}
