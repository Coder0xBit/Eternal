#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Eternal {
    void VulkanBuffer::create(uint32_t elementCount, uint32_t elementSize, vk::BufferUsageFlagBits usage) {
        m_ElementCount = elementCount;
        m_BufferSize = elementSize * m_ElementCount;

        vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
                .setSize(m_BufferSize)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);

        m_Buffer = m_LogicalDevice.createBuffer(bufferInfo);
    }

    void VulkanBuffer::allocate(vk::MemoryPropertyFlags properties) {
        vk::MemoryRequirements memRequirements = m_LogicalDevice.getBufferMemoryRequirements(m_Buffer);

        uint32_t memoryTypeIndex = VulkanPlatform::getMemoryType(m_PhysicalDevice, properties,
                                                                 memRequirements.memoryTypeBits);
        ETERNAL_ASSERT(memoryTypeIndex != 0xFFFFFFFF, "Failed to find suitable memory type");

        vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
                .setAllocationSize(memRequirements.size)
                .setMemoryTypeIndex(memoryTypeIndex);

        m_Memory = m_LogicalDevice.allocateMemory(allocInfo);
        m_LogicalDevice.bindBufferMemory(m_Buffer, m_Memory, 0);
    }

    void VulkanBuffer::map() {
        if (!m_MappedMemory) {
            m_MappedMemory = m_LogicalDevice.mapMemory(m_Memory, 0, m_BufferSize);
        }
    }

    void VulkanBuffer::unMap() {
        m_LogicalDevice.unmapMemory(m_Memory);
        m_MappedMemory = nullptr;
    }

    void VulkanBuffer::write(void* data) {
        ETERNAL_ASSERT(m_MappedMemory != nullptr, "Memory should be mapped first, forgot call map(..) maybe ?");
        memcpy(m_MappedMemory, data, (size_t) m_BufferSize);
    }

    VulkanBuffer::~VulkanBuffer() {
        if (isCurrentlyMapped()) {
            Eternal::Logger::Info("Buffer was previously mapped, internally unmapping");
            unMap();
        }

        if (m_Memory) {
            m_LogicalDevice.freeMemory(m_Memory);
        }

        if (m_Buffer) {
            m_LogicalDevice.destroyBuffer(m_Buffer);
        }

        m_MappedMemory = nullptr;
    }
}
