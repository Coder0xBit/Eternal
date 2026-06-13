#include <utility>

#include "core/graphics/vulkan/VulkanVertexBuffer.h"

namespace Vortak {
    VulkanVertexBuffer::VulkanVertexBuffer(
        GraphicsPlatform* graphicsPlatform,
        VertexBufferLayout bufferLayout
    ) : VertexBuffer(std::move(bufferLayout)) {
        mVulkanPlatform = static_cast<VulkanPlatform*>(graphicsPlatform);
        mBuffer = std::make_unique<VulkanBuffer>(mVulkanPlatform);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
    }

    void VulkanVertexBuffer::bind() {
    }

    void VulkanVertexBuffer::unBind() {
    }

    void VulkanVertexBuffer::setBuffer(const std::vector<Vortak::Vertex>& vertices) {
        mBuffer->create(vertices.size(), sizeof(Vortak::Vertex),
                        vk::BufferUsageFlagBits::eVertexBuffer);
        mBuffer->allocate(mBufferProperties);
        mBuffer->map();
        mBuffer->write((void*) (vertices.data()));
    }
}
