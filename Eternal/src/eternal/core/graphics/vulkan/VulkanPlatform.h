#pragma once

#include <eternal/core/graphics/GraphicsPlatform.h>
#include <eternal/utils/Base.h>
#include <eternal/core/Window.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <fstream>
#include <filesystem>

namespace Eternal {

	constexpr uint32_t const INVALID_VK_INDEX = 0xFFFFFFFF;

	constexpr uint32_t const MAX_FRAMES_IN_FLIGHT = 2;

	class VulkanPlatform : public GraphicsPlatform
	{

	public:

		using VkStringArray = std::vector<const char*>;
		using VkString = const char*;
		using VkStringArrayPtr = const char**;

		struct Buffer
		{
			vk::Buffer handle = nullptr;
			vk::DeviceMemory memory = nullptr;
			vk::DeviceSize size = 0;
			vk::BufferUsageFlagBits usage = vk::BufferUsageFlagBits::eIndexBuffer;
		};

		struct SwapChainDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

		struct SwapChainCreateDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities;
			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			vk::Extent2D extent;
		};

		VulkanPlatform(std::string& applicationName, Eternal::Window* window);

		~VulkanPlatform();

		void initialize() override;

		void shutDown() override;

		void render() override;

		vk::Instance createInstance(const std::string& applicationName);

		vk::PhysicalDevice choosePhysicalDevice(vk::Instance& instance);

		uint32_t identifyGraphicsQueueFamilyIndex(vk::PhysicalDevice& device, vk::QueueFlags flags);

		uint32_t identifyPresentQueueFamilyIndex(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);

		vk::Device createLogicalDevice(vk::PhysicalDevice& device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex);

		SwapChainDetails querySwapChainDetails(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);

		vk::SwapchainKHR createSwapChain(
			const vk::Device& logicalDevice,
			const SwapChainCreateDetails& swapChainCreateDetails,
			vk::SurfaceKHR& surface,
			uint32_t graphicsQueueFamilyIndex,
			uint32_t presentQueueFamilyIndex
		);

		SwapChainCreateDetails getSwapChainCreateDetails(const SwapChainDetails& swapChainDetails);

		vk::SurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

		vk::PresentModeKHR chooseSwapChainPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

		vk::Extent2D chooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

		vk::RenderPass createRenderPass(const vk::Device& logicalDevice);

		vk::ShaderModule loadShader(const vk::Device& logicalDevice, const std::filesystem::path& path);

		bool validateExtensions(VkStringArray extensions);

		bool validateLayers(VkStringArray layers);

		bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray requestedExtensions);

		bool checkDeviceIsSuitable(vk::PhysicalDevice& device);

		void logDeviceProps(const vk::PhysicalDevice& device);

		void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

	private:

		void initializeSwapChain();

		void initializePipeline();

		void initializeRenderPass();

		void initializeFrameBuffers();

		void initializeCommandPool();

		void initializeCommandBuffer();

		void initializeSyncObjects();

		void cleanupSwapChain();

		void recreateSwapChain();

		std::string m_ApplicationName;

		Eternal::Window* m_Window = nullptr;

		vk::Instance m_VkInstance = nullptr;
		vk::PhysicalDevice m_VkPhysicalDevice = nullptr;
		vk::Device m_LogicalDevice = nullptr;
		vk::SurfaceKHR m_Surface = nullptr;

		vk::SwapchainKHR m_SwapChain = nullptr;
		SwapChainCreateDetails m_SwapChainCreateDetails = { };
		std::vector<vk::Image> m_SwapChainImages;
		std::vector<vk::ImageView> m_SwapChainImageViews;

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

		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
		std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
		std::vector<vk::Fence> m_InFlightFences;

		uint32_t m_CurrentFrame = 0;
	};
}