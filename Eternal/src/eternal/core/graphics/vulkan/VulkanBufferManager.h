#pragma once

#include <eternal/core/ecs/EntityManager.h>
#include <eternal/utils/Base.h>

#include <vulkan/vulkan.hpp>

namespace Eternal {
	class VulkanBufferManager {
	public:

		using EntityId = uint32_t;

		struct Buffer
		{
			vk::Buffer handle = nullptr;
			vk::DeviceMemory memory = nullptr;
			vk::DeviceSize size = 0;
			uint32_t count = 0;
			vk::BufferUsageFlagBits usage = vk::BufferUsageFlagBits::eIndexBuffer;
		};

		VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, Memory::Ref<Eternal::EntityManager> entityManager);

		void bindBuffers(vk::CommandBuffer commandBuffer);

		void draw(vk::CommandBuffer commandBuffer);

		~VulkanBufferManager();

	private:

		void createOrResizeBuffer(Buffer& buffer, uint32_t newSize);

		void initializeBuffers();

		uint32_t getMemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits);

		Memory::Ref<Eternal::EntityManager> m_EntityManager;

		vk::Device m_Device;

		vk::PhysicalDevice m_PhysicalDevice;

		std::unordered_map<EntityId, Memory::Ref<Buffer>> m_VertexBuffers;

		std::unordered_map<EntityId, Memory::Ref<Buffer>> m_IndexBuffers;
	};
}