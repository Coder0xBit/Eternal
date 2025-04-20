#include "VulkanBufferManager.h"
#include <eternal/core/ecs/RenderComponent.h>

namespace Eternal {

	VulkanBufferManager::VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, EntityManager* entityManager)
	{
		m_Device = device;
		m_EntityManager = entityManager;
		m_PhysicalDevice = physicalDevice;

		initializeBuffers();
	}

	void VulkanBufferManager::bindBuffers(vk::CommandBuffer commandBuffer)
	{
		for (auto& [entityId, buffer] : m_VertexBuffers)
		{
			vk::DeviceSize offset = vk::DeviceSize(0);
			commandBuffer.bindVertexBuffers(0, 1, &(buffer->handle), &offset);
		}
		for (auto& [entityId, buffer] : m_IndexBuffers)
		{
			commandBuffer.bindIndexBuffer(buffer->handle, 0, vk::IndexType::eUint32);
		}
	}

	void VulkanBufferManager::draw(vk::CommandBuffer commandBuffer)
	{
		for (auto& [entityId, buffer] : m_IndexBuffers)
		{
			commandBuffer.drawIndexed(buffer->count, 1, 0, 0, 0);
		}
	}

	VulkanBufferManager::~VulkanBufferManager()
	{
		for (auto& [entityId, buffer] : m_VertexBuffers) {
			if (buffer->handle)
				m_Device.destroyBuffer(buffer->handle);

			if (buffer->memory)
				m_Device.freeMemory(buffer->memory);
		}

		for (auto& [entityId, buffer] : m_IndexBuffers) {
			if (buffer->handle)
				m_Device.destroyBuffer(buffer->handle);

			if (buffer->memory)
				m_Device.freeMemory(buffer->memory);
		}
	}

	void VulkanBufferManager::initializeBuffers()
	{
		for (auto& [entityId, component] : m_EntityManager->getComponentStorage<Eternal::RenderComponent>())
		{
			auto vertexBuffer = std::make_shared<Buffer>();

			vertexBuffer->count = component.getVertices().size();
			uint32_t vertexBufferSize = sizeof(Eternal::Vertex) * vertexBuffer->count;
			vertexBuffer->usage = vk::BufferUsageFlagBits::eVertexBuffer;
			createOrResizeBuffer(*vertexBuffer, vertexBufferSize);

			Eternal::Vertex* vertexBufferMemory = static_cast<Eternal::Vertex*>(m_Device.mapMemory(vertexBuffer->memory, 0, vertexBufferSize));
			memcpy(vertexBufferMemory, component.getVertices().data(), vertexBufferSize);

			m_VertexBuffers[entityId] = vertexBuffer;

			auto indexBuffer = std::make_shared<Buffer>();

			indexBuffer->count = component.getIndices().size();
			uint32_t indexBufferSize = sizeof(uint32_t) * indexBuffer->count;
			indexBuffer->usage = vk::BufferUsageFlagBits::eIndexBuffer;
			createOrResizeBuffer(*indexBuffer, indexBufferSize);

			uint32_t* indexBufferMemory = static_cast<uint32_t*>(m_Device.mapMemory(indexBuffer->memory, 0, indexBufferSize));
			memcpy(indexBufferMemory, component.getIndices().data(), indexBufferSize);

			m_IndexBuffers[entityId] = indexBuffer;

			m_Device.unmapMemory(vertexBuffer->memory);
			m_Device.unmapMemory(indexBuffer->memory);
		}
	}

	void VulkanBufferManager::createOrResizeBuffer(Buffer& buffer, uint32_t newSize)
	{
		if (buffer.handle)
			m_Device.destroyBuffer(buffer.handle);

		if (buffer.memory)
			m_Device.freeMemory(buffer.memory);

		vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
			.setSize(newSize)
			.setUsage(buffer.usage)
			.setSharingMode(vk::SharingMode::eExclusive);

		buffer.handle = m_Device.createBuffer(bufferCreateInfo);

		vk::MemoryRequirements memoryRequirements = m_Device.getBufferMemoryRequirements(buffer.handle);

		vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memoryRequirements.size)
			.setMemoryTypeIndex(getMemoryType(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, memoryRequirements.memoryTypeBits));

		buffer.memory = m_Device.allocateMemory(memoryAllocateInfo);

		m_Device.bindBufferMemory(buffer.handle, buffer.memory, 0);

		buffer.size = memoryRequirements.size;
	}

	uint32_t VulkanBufferManager::getMemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits)
	{
		vk::PhysicalDeviceMemoryProperties prop = m_PhysicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
				return i;
		return 0xFFFFFFFF;
	}
}

