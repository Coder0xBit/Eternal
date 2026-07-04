#pragma once

#include <vulkan/vulkan.hpp>

#include "core/graphics/vulkan/VulkanPlatform.h"
#include "utils/Base.h"
#include "core/log/Logger.h"

namespace Vortak {
    class VulkanBuffer {
    public:
        VulkanBuffer(VulkanPlatform* vulkanPlatform) : mVulkanPlatform(vulkanPlatform) {
        }

        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;
        ~VulkanBuffer();

        void create(uint32_t elementCount, uint32_t elementSize, vk::BufferUsageFlagBits usage);
        void allocate(vk::MemoryPropertyFlags properties);
        void map();
        void unMap();
        void write(void* data);
        bool isCurrentlyMapped() const { return mMappedMemory != nullptr; }
        void* mappedMemory() const { return mMappedMemory; }
        const uint32_t& getElementCount() const { return mElementCount; }
        uint32_t getBufferSize() const { return mBufferSize; }

        vk::Buffer* getVkBuffer() {
            VORTAK_ASSERT_LOG(mBuffer, "Buffer is not created yet, call create() first");
            return &mBuffer;
        }

    private:
        vk::Buffer mBuffer = nullptr;
        VulkanPlatform* mVulkanPlatform = nullptr;
        vk::DeviceMemory mMemory = nullptr;
        void* mMappedMemory = nullptr;
        uint32_t mElementCount = 0;
        uint32_t mBufferSize = 0;
    };
}
