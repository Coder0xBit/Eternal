#pragma once

#include <vulkan/vulkan.hpp>
#include <eternal/utils/Base.h>
#include <eternal/core/Logger.h>


namespace Eternal {
	class VulkanBuffer {
	public:

		VulkanBuffer(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice) :
			m_LogicalDevice(logicalDevice), m_PhysicalDevice(physicalDevice) {
		}

		VulkanBuffer(const VulkanBuffer&) = delete;

		VulkanBuffer& operator=(const VulkanBuffer&) = delete;

		void create(uint32_t elementCount, uint32_t elementSize, vk::BufferUsageFlagBits usage);

		void allocate(vk::MemoryPropertyFlags properties);

		void map();

		void unMap();

		void write(void* data);

		~VulkanBuffer();

		bool isCurrentlyMapped() const { return m_MappedMemory != nullptr; }

		void* mappedMemory() const { return m_MappedMemory; }

		const uint32_t& getElementCount() const { return m_ElementCount; }

		uint32_t getBufferSize() const { return m_BufferSize; }

		vk::Buffer* getBuffer() { return &m_Buffer; }

	private:
		vk::Device m_LogicalDevice = nullptr;
		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		vk::Buffer m_Buffer = nullptr;
		vk::DeviceMemory m_Memory = nullptr;
		void* m_MappedMemory = nullptr;
		uint32_t m_ElementCount = 0;
		uint32_t m_BufferSize = 0;
	};
}