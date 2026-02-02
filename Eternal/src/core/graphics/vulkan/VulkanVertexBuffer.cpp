#include "core/graphics/vulkan/VulkanVertexBuffer.h"

namespace Eternal {
    VulkanVertexBuffer::VulkanVertexBuffer(VulkanPlatform* vulkanPlatform,
                                           const std::vector<Eternal::Vertex>& vertices)
        : m_VulkanPlatform(vulkanPlatform) {
        m_Buffer = std::make_unique<VulkanBuffer>(m_VulkanPlatform);
        m_Buffer->create(vertices.size(), sizeof(Eternal::Vertex),
                           vk::BufferUsageFlagBits::eVertexBuffer);
        m_Buffer->allocate(m_BufferProperties);
        m_Buffer->map();
        m_Buffer->write((void*) (vertices.data()));
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {

    }

    void VulkanVertexBuffer::bind() {
    }

    void VulkanVertexBuffer::unBind() {
    }
}
