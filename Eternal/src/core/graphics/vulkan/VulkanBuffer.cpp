#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Eternal {
    void VulkanBuffer::create(uint32_t elementCount, uint32_t elementSize, vk::BufferUsageFlagBits usage) {
        vk::Device logicalDevice = m_VulkanPlatform->getLogicalDevice();
        m_ElementCount = elementCount;
        m_BufferSize = elementSize * m_ElementCount;

        vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
                .setSize(m_BufferSize)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);

        m_Buffer = logicalDevice.createBuffer(bufferInfo);
    }

    void VulkanBuffer::allocate(vk::MemoryPropertyFlags properties) {
        vk::Device logicalDevice = m_VulkanPlatform->getLogicalDevice();
        vk::PhysicalDevice physicalDevice = m_VulkanPlatform->getPhysicalDevice();

        vk::MemoryRequirements memRequirements = logicalDevice.getBufferMemoryRequirements(m_Buffer);

        uint32_t memoryTypeIndex = VulkanPlatform::getMemoryType(physicalDevice, properties,
                                                                 memRequirements.memoryTypeBits);
        ETERNAL_ASSERT(memoryTypeIndex != 0xFFFFFFFF, "Failed to find suitable memory type");

        vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
                .setAllocationSize(memRequirements.size)
                .setMemoryTypeIndex(memoryTypeIndex);

        m_Memory = logicalDevice.allocateMemory(allocInfo);
        logicalDevice.bindBufferMemory(m_Buffer, m_Memory, 0);
    }

    void VulkanBuffer::map() {
        vk::Device logicalDevice = m_VulkanPlatform->getLogicalDevice();
        if (!m_MappedMemory) {
            m_MappedMemory = logicalDevice.mapMemory(m_Memory, 0, m_BufferSize);
        }
    }

    void VulkanBuffer::unMap() {
        vk::Device logicalDevice = m_VulkanPlatform->getLogicalDevice();
        logicalDevice.unmapMemory(m_Memory);
        m_MappedMemory = nullptr;
    }

    void VulkanBuffer::write(void* data) {
        ETERNAL_ASSERT(m_MappedMemory != nullptr, "Memory should be mapped first, forgot call map(..) maybe ?");
        memcpy(m_MappedMemory, data, (size_t) m_BufferSize);
    }

    VulkanBuffer::~VulkanBuffer() {
        vk::Device logicalDevice = m_VulkanPlatform->getLogicalDevice();
        if (isCurrentlyMapped()) {
            Eternal::Logger::Info("Buffer was previously mapped, internally unmapping");
            unMap();
        }

        if (m_Memory) {
            logicalDevice.freeMemory(m_Memory);
        }

        if (m_Buffer) {
            logicalDevice.destroyBuffer(m_Buffer);
        }

        m_MappedMemory = nullptr;
    }
}
