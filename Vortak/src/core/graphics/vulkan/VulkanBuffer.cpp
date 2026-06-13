#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Vortak {
    void VulkanBuffer::create(uint32_t elementCount, uint32_t elementSize, vk::BufferUsageFlagBits usage) {
        vk::Device logicalDevice = mVulkanPlatform->getLogicalDevice();
        mElementCount = elementCount;
        mBufferSize = elementSize * mElementCount;

        vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
                .setSize(mBufferSize)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);

        mBuffer = logicalDevice.createBuffer(bufferInfo);
    }

    void VulkanBuffer::allocate(vk::MemoryPropertyFlags properties) {
        vk::Device logicalDevice = mVulkanPlatform->getLogicalDevice();
        vk::PhysicalDevice physicalDevice = mVulkanPlatform->getPhysicalDevice();

        vk::MemoryRequirements memRequirements = logicalDevice.getBufferMemoryRequirements(mBuffer);

        uint32_t memoryTypeIndex = VulkanPlatform::getMemoryType(physicalDevice, properties,
                                                                 memRequirements.memoryTypeBits);
        Vortak_ASSERT(memoryTypeIndex != 0xFFFFFFFF, "Failed to find suitable memory type");

        vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
                .setAllocationSize(memRequirements.size)
                .setMemoryTypeIndex(memoryTypeIndex);

        mMemory = logicalDevice.allocateMemory(allocInfo);
        logicalDevice.bindBufferMemory(mBuffer, mMemory, 0);
    }

    void VulkanBuffer::map() {
        vk::Device logicalDevice = mVulkanPlatform->getLogicalDevice();
        if (!mMappedMemory) {
            mMappedMemory = logicalDevice.mapMemory(mMemory, 0, mBufferSize);
        }
    }

    void VulkanBuffer::unMap() {
        vk::Device logicalDevice = mVulkanPlatform->getLogicalDevice();
        logicalDevice.unmapMemory(mMemory);
        mMappedMemory = nullptr;
    }

    void VulkanBuffer::write(void* data) {
        Vortak_ASSERT(mMappedMemory != nullptr, "Memory should be mapped first, forgot call map(..) maybe ?");
        memcpy(mMappedMemory, data, (size_t) mBufferSize);
    }

    VulkanBuffer::~VulkanBuffer() {
        vk::Device logicalDevice = mVulkanPlatform->getLogicalDevice();
        if (isCurrentlyMapped()) {
            Vortak::Logger::Info("Buffer was previously mapped, internally unmapping");
            unMap();
        }

        if (mMemory) {
            logicalDevice.freeMemory(mMemory);
        }

        if (mBuffer) {
            logicalDevice.destroyBuffer(mBuffer);
        }

        mMappedMemory = nullptr;
    }
}
