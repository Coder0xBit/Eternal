#pragma once
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/graphics/VertexBufferLayout.h"
#include "core/graphics/Vertex.h"
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Vortak {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(GraphicsPlatform* graphicsPlatform, VertexBufferLayout bufferLayout);
        ~VulkanVertexBuffer() override;
        void bind() override;
        void unBind() override;
        void setBuffer(const std::vector<Vortak::Vertex>& vertices) override;
        uint32_t getSize() override { return mBuffer->getBufferSize(); }

        // the lifetime of mBuffer is handled by the VulkanVertexBuffer, do not delete after acquiring it
        VulkanBuffer* getVulkanBuffer() { return mBuffer.get(); }

    private :
        std::unique_ptr<VulkanBuffer> mBuffer;
        vk::MemoryPropertyFlags mBufferProperties =
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        VulkanPlatform* mVulkanPlatform;
    };
}
