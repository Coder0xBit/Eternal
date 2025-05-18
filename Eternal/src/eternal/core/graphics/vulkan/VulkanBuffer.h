#pragma once

#include <vulkan/vulkan.hpp>
#include <eternal/utils/Base.h>
#include "VulkanPlatform.h"

namespace Eternal {
	class VulkanBuffer {
	public:

		VulkanBuffer(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice) :
			m_LogicalDevice(logicalDevice), m_PhysicalDevice(physicalDevice) {
		}

		template<typename T>
		void create(const std::vector<T>& bufferData, vk::BufferUsageFlagBits usage, vk::MemoryPropertyFlags properties)
		{
			m_ElementCount = static_cast<uint32_t>(bufferData.size());
			m_BufferSize = sizeof(T) * m_ElementCount;

			vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
				.setSize(m_BufferSize)
				.setUsage(usage)
				.setSharingMode(vk::SharingMode::eExclusive);

			m_Buffer = m_LogicalDevice.createBuffer(bufferInfo);

			vk::MemoryRequirements memRequirements = m_LogicalDevice.getBufferMemoryRequirements(m_Buffer);

			uint32_t memoryTypeIndex = VulkanPlatform::getMemoryType(m_PhysicalDevice, properties, memRequirements.memoryTypeBits);
			ETERNAL_ASSERT(memoryTypeIndex != 0xFFFFFFFF, "Failed to find suitable memory type");

			vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
				.setAllocationSize(memRequirements.size)
				.setMemoryTypeIndex(memoryTypeIndex);

			m_Memory = m_LogicalDevice.allocateMemory(allocInfo);
			m_LogicalDevice.bindBufferMemory(m_Buffer, m_Memory, 0);

			void* data = m_LogicalDevice.mapMemory(m_Memory, 0, m_BufferSize);
			memcpy(data, bufferData.data(), (size_t)m_BufferSize);
			m_LogicalDevice.unmapMemory(m_Memory);
		}

		~VulkanBuffer()
		{
			if (m_Memory)
			{
				m_LogicalDevice.freeMemory(m_Memory);
			}
			if (m_Buffer)
			{
				m_LogicalDevice.destroyBuffer(m_Buffer);
			}
		}

		bool isCurrentlyMapped() const { return m_Memory; }

		const uint32_t& getElementCount() const { return m_ElementCount; }

		uint32_t getBufferSize() const { return m_BufferSize; }

		vk::Buffer* getBuffer() { return &m_Buffer; }

	private:
		vk::Device m_LogicalDevice = nullptr;
		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		vk::Buffer m_Buffer = nullptr;
		vk::DeviceMemory m_Memory = nullptr;
		uint32_t m_ElementCount = 0;
		uint32_t m_BufferSize = 0;
	};
}