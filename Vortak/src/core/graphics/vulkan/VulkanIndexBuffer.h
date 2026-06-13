#pragma once
#include "core/graphics/IndexBuffer.h"
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/graphics/Vertex.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Vortak {
    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(GraphicsPlatform* graphicsPlatform);
        ~VulkanIndexBuffer() override;
        void bind() override;
        void unBind() override;
        uint32_t getCount() override { return mBuffer->getBufferSize(); }
        void setBuffer(const std::vector<uint32_t>& indices) override;

        // the lifetime of mBuffer is handled by the VulkanIndexBuffer, do not delete after acquiring it
        VulkanBuffer* getVulkanBuffer() { return mBuffer.get(); }

    private :
        std::unique_ptr<VulkanBuffer> mBuffer;
        vk::MemoryPropertyFlags mBufferProperties =
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        VulkanPlatform* mVulkanPlatform;
    };
}
