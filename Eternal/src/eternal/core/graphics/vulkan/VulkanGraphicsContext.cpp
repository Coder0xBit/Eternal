#include "VulkanGraphicsContext.h"


namespace Eternal {

	VulkanGraphicsContext::VulkanGraphicsContext(
		vk::Instance instance,
		vk::PhysicalDevice physicalDevice,
		vk::Device logicalDevice,
		vk::Queue graphicsQueue,
		uint32_t graphicsQueueFamilyIndex
	) : m_VkInstance(instance),
		m_PhysicalDevice(physicalDevice),
		m_LogicalDevice(logicalDevice),
		m_GraphicsQueue(graphicsQueue),
		m_GraphicsQueueFamilyIndex(graphicsQueueFamilyIndex)
	{
	
	}
	VulkanGraphicsContext::~VulkanGraphicsContext()
	{

	}
}
