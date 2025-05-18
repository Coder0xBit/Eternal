#include "VulkanBufferManager.h"
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/scene/RenderComponent.h>
#include <eternal/core/scene/Entity.h>

namespace Eternal {

	VulkanBufferManager::VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, Scene* scene)
	{
		m_Device = device;
		m_Scene = scene;
		m_PhysicalDevice = physicalDevice;

		initializeBuffers();
	}

	void VulkanBufferManager::bindBuffers(vk::CommandBuffer commandBuffer)
	{
		for (auto& [entityId, buffer] : m_VertexBuffers)
		{
			vk::DeviceSize offset = vk::DeviceSize(0);
			commandBuffer.bindVertexBuffers(0, 1, buffer->getBuffer(), &offset);
		}
		for (auto& [entityId, buffer] : m_IndexBuffers)
		{
			commandBuffer.bindIndexBuffer(*(buffer->getBuffer()), 0, vk::IndexType::eUint32);
		}
	}

	void VulkanBufferManager::draw(vk::CommandBuffer commandBuffer)
	{
		for (auto& [entityId, buffer] : m_IndexBuffers)
		{
			commandBuffer.drawIndexed(buffer->getElementCount(), 1, 0, 0, 0);
		}
	}

	VulkanBufferManager::~VulkanBufferManager()
	{
		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
	}

	void VulkanBufferManager::initializeBuffers()
	{
		vk::MemoryPropertyFlags bufferProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		for (auto& e : m_Scene->getAllEntityWith<Eternal::RenderComponent>())
		{
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);
			auto& component = entity.getComponent<Eternal::RenderComponent>();

			auto vertexBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
			vertexBuffer->create(component.getVertices(), vk::BufferUsageFlagBits::eVertexBuffer, bufferProperties);

			m_VertexBuffers[entity.getUUID()] = vertexBuffer;

			auto indexBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
			indexBuffer->create(component.getIndices(), vk::BufferUsageFlagBits::eIndexBuffer, bufferProperties);

			m_IndexBuffers[entity.getUUID()] = indexBuffer;
		}
	}
}

