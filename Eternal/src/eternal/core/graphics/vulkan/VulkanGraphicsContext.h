#pragma once

#include <eternal/core/graphics/GraphicsContext.h>
#include <eternal/core/graphics/vulkan/VulkanSwapChain.h>
#include <eternal/core/Window.h>

#include <vulkan/vulkan.hpp>

namespace Eternal {
	class VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext();

		~VulkanGraphicsContext();

		virtual vk::CommandBuffer getCommandBuffer() = 0;

		vk::Instance getVkInstance() { return m_VkInstance; }

		vk::PhysicalDevice getPhysicalDevice() { return m_PhysicalDevice; }

		vk::Device getLogicalDevice() { return m_LogicalDevice; }

		vk::Queue getGraphicsQueue() { return m_GraphicsQueue; }

		uint32_t getGraphicsQueueFamilyIndex() { return m_GraphicsQueueFamilyIndex; }

		vk::RenderPass getRenderPass() { return m_RenderPass; }

		Eternal::Window* getWindow() { return m_Window; }

	protected:

		VulkanSwapChain* m_VulkanSwapChain = nullptr;

		vk::Instance m_VkInstance = nullptr;

		Eternal::Window* m_Window = nullptr;

		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		vk::Device m_LogicalDevice = nullptr;

		vk::PipelineLayout m_PipelineLayout = nullptr;
		vk::Pipeline m_GraphicsPipeline = nullptr;
		vk::RenderPass m_RenderPass = nullptr;

		std::vector<vk::Framebuffer> m_SwapChainFrameBuffers;

		vk::CommandPool m_CommandPool = nullptr;
		std::vector<vk::CommandBuffer> m_CommandBuffers;

		vk::Queue m_GraphicsQueue = nullptr;
		uint32_t m_GraphicsQueueFamilyIndex = INVALID_VK_INDEX;
		uint32_t m_GraphicsQueueIndex = 0;

		vk::Queue m_PresentQueue = nullptr;
		uint32_t m_PresentQueueFamilyIndex = INVALID_VK_INDEX;
		uint32_t m_PresentQueueIndex = 0;

	};
}