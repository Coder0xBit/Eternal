#pragma once

#include <vulkan/vulkan.hpp>

#include "core/graphics/vulkan/VulkanPlatform.h"
#include "utils/Base.h"
#include "core/log/Logger.h"

namespace Eternal {
    class VulkanBuffer {
    public:
        VulkanBuffer(VulkanPlatform* vulkanPlatform) : m_VulkanPlatform(vulkanPlatform) {
        }

        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;
        ~VulkanBuffer();

        void create(uint32_t elementCount, uint32_t elementSize, vk::BufferUsageFlagBits usage);
        void allocate(vk::MemoryPropertyFlags properties);
        void map();
        void unMap();
        void write(void* data);
        bool isCurrentlyMapped() const { return m_MappedMemory != nullptr; }
        void* mappedMemory() const { return m_MappedMemory; }
        const uint32_t& getElementCount() const { return m_ElementCount; }
        uint32_t getBufferSize() const { return m_BufferSize; }

        vk::Buffer* getVkBuffer() {
            ETERNAL_ASSERT_LOG(m_Buffer, "Buffer is not created yet, call create() first");
            return &m_Buffer;
        }

    private:
        vk::Buffer m_Buffer = nullptr;
        VulkanPlatform* m_VulkanPlatform = nullptr;
        vk::DeviceMemory m_Memory = nullptr;
        void* m_MappedMemory = nullptr;
        uint32_t m_ElementCount = 0;
        uint32_t m_BufferSize = 0;
    };
}
