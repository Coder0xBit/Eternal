#pragma once

#include <eternal/core/scene/Scene.h>
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

		VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, Scene* scene);

		void bindBuffers(vk::CommandBuffer commandBuffer);

		void draw(vk::CommandBuffer commandBuffer);

		~VulkanBufferManager();

	private:

		void createOrResizeBuffer(Buffer& buffer, uint32_t newSize);

		void initializeBuffers();

		uint32_t getMemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits);

		Scene* m_Scene;

		vk::Device m_Device;

		vk::PhysicalDevice m_PhysicalDevice;

		std::unordered_map<EntityId, std::shared_ptr<Buffer>> m_VertexBuffers;

		std::unordered_map<EntityId, std::shared_ptr<Buffer>> m_IndexBuffers;
	};
}