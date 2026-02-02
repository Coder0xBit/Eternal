#pragma once
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/graphics/Vertex.h"
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Eternal {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(VulkanPlatform* vulkanPlatform, const std::vector<Eternal::Vertex>& vertices);
        ~VulkanVertexBuffer() override;
        void bind() override;
        void unBind() override;
        uint32_t getCount() override { return m_Buffer->getBufferSize(); };
        vk::Buffer* getVkBuffer() const { return m_Buffer->getVkBuffer(); }

    private :
        std::unique_ptr<VulkanBuffer> m_Buffer;
        vk::MemoryPropertyFlags m_BufferProperties =
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        VulkanPlatform* m_VulkanPlatform;
    };
}
