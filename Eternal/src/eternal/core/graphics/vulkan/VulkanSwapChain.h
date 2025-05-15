#pragma once
#include <eternal/utils/Base.h>
#include <vulkan/vulkan.hpp>
#include <eternal/core/graphics/SwapChain.h>

namespace Eternal {
	class VulkanSwapChain : public SwapChain
	{
	public:
		struct SwapChainDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities;
			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			vk::Extent2D extent;
		};

		VulkanSwapChain(
			vk::Instance instance,
			vk::Device logicalDevice,
			vk::PhysicalDevice physicalDevice,
			vk::Queue queue,
			vk::SurfaceKHR surface,
			vk::Extent2D extent,
			uint32_t graphicsQueueFamilyIndex,
			uint32_t presentQueueFamilyIndex
		);

		~VulkanSwapChain();

		vk::Result acquire(vk::Semaphore imageReadySemaphore, uint32_t* imageIndex);

		vk::Result present(vk::Semaphore renderFinishedSemaphore, uint32_t imageIndex);

		void recreate();

		const SwapChainDetails& getSwapChainDetails() { return m_SwapChainDetails; }

		const std::vector<vk::ImageView>& getImageViews() { return m_SwapChainImageViews; }

		const std::vector<vk::Framebuffer>& getFrameBuffers() { return m_SwapChainFrameBuffers; }

		const std::vector<vk::Image>& getImages() { return m_SwapChainImages; }

		vk::SwapchainKHR getSwapChain() { return m_SwapChain; }

		vk::RenderPass getRenderPass() { return m_RenderPass; }

		vk::SurfaceKHR getSurface() { return m_Surface; }

		bool shouldRecreate() const { return m_ShouldRecreate; }

		void setShouldRecreate(bool shouldRecreate) { m_ShouldRecreate = shouldRecreate; }

		void destroy();

	private:
		void create();

		vk::SurfaceFormatKHR selectSwapChainSurfaceFormat();

		vk::PresentModeKHR selectSwapChainPresentMode();

		vk::Extent2D selectSwapChainExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

		void createImageViews();

		void createDepthImageView();

		void createRenderPass();

		void createFrameBuffers();

		uint32_t getMemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits);

		bool m_ShouldRecreate = false;

		vk::Instance m_VkInstance = nullptr;

		vk::Device m_LogicalDevice = nullptr;

		vk::PhysicalDevice m_PhysicalDevice = nullptr;

		vk::Queue m_PresentQueue = nullptr;

		uint32_t m_GraphicsQueueFamilyIndex = INVALID_VK_INDEX;

		uint32_t m_PresentQueueFamilyIndex = INVALID_VK_INDEX;

		vk::SurfaceKHR m_Surface = nullptr;

		vk::Extent2D m_FallBackExtent;

		vk::RenderPass m_RenderPass = nullptr;

		std::vector<vk::Framebuffer> m_SwapChainFrameBuffers;

		std::vector<vk::Image> m_SwapChainImages;

		std::vector<vk::ImageView> m_SwapChainImageViews;

		vk::Image m_DepthImage;

		vk::DeviceMemory m_DepthImageMemory;

		vk::ImageView m_DepthImageView;

		vk::SwapchainKHR m_SwapChain = nullptr;

		SwapChainDetails m_SwapChainDetails = {};
	};
}