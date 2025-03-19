#pragma once

#include <eternal/core/graphics/GraphicsContext.h>
#include <eternal/core/graphics/vulkan/VulkanSwapChain.h>
#include <vulkan/vulkan.hpp>

namespace Eternal {
	class VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(
			vk::Instance instance,
			vk::PhysicalDevice physicalDevice,
			vk::Device logicalDevice,
			vk::Queue graphicsQueue,
			uint32_t graphicsQueueFamilyIndex
		);

		~VulkanGraphicsContext();

		vk::Instance getVkInstance() { return m_VkInstance; }

		vk::PhysicalDevice getPhysicalDevice() { return m_PhysicalDevice; }

		vk::Device getLogicalDevice() { return m_LogicalDevice; }

		vk::Queue getGraphicsQueue() { return m_GraphicsQueue; }

		uint32_t getGraphicsQueueFamilyIndex() { return m_GraphicsQueueFamilyIndex; }

	private:

		VulkanSwapChain* m_VulkanSwapChain = nullptr;
		vk::Instance m_VkInstance = nullptr;
		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		vk::Device m_LogicalDevice = nullptr;
		uint32_t m_GraphicsQueueFamilyIndex = INVALID_VK_INDEX;
		vk::Queue m_GraphicsQueue = nullptr;
	};
}