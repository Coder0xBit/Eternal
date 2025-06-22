#pragma once

#include <core/scene/Scene.h>
#include <utils/Base.h>
#include <core/scene/RenderComponent.h>

#include "VulkanBuffer.h"

namespace Eternal {

	class VulkanPlatform;

	class VulkanBufferManager {
	public:

		using EntityId = uint32_t;

		VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, Scene* scene);

		std::shared_ptr<VulkanBuffer> getVertexBuffer(EntityId entityId) {
			auto it = m_VertexBuffers.find(entityId);
			if (it != m_VertexBuffers.end()) {
				return it->second;
			}
			return nullptr;
		}

		std::shared_ptr<VulkanBuffer> getIndexBuffer(EntityId entityId) {
			auto it = m_IndexBuffers.find(entityId);
			if (it != m_IndexBuffers.end()) {
				return it->second;
			}
			return nullptr;
		}

		void addBuffer(EntityId entityId, const RenderComponent& renderComponent);

		~VulkanBufferManager();

	private:

		void initializeBuffers();

		Scene* m_Scene;

		vk::Device m_Device;

		vk::PhysicalDevice m_PhysicalDevice;
		vk::MemoryPropertyFlags m_BufferProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>> m_VertexBuffers;
		std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>> m_IndexBuffers;
	};
}