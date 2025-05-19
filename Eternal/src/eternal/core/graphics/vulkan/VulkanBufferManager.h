#pragma once

#include <eternal/core/scene/Scene.h>
#include <eternal/utils/Base.h>

#include "VulkanBuffer.h"

namespace Eternal {

	class VulkanPlatform;

	class VulkanBufferManager {
	public:

		using EntityId = uint32_t;

		VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, Scene* scene);

		void bindBuffers(vk::CommandBuffer commandBuffer);

		void draw(vk::CommandBuffer commandBuffer);

		~VulkanBufferManager();

	private:

		void initializeBuffers();

		Scene* m_Scene;

		vk::Device m_Device;

		vk::PhysicalDevice m_PhysicalDevice;

		std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>> m_VertexBuffers;

		std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>> m_IndexBuffers;
	};
}