#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <eternal/core/graphics/GraphicsPlatform.h>
#include <eternal/utils/Base.h>
#include <eternal/core/Window.h>
#include <eternal/core/graphics/Timer.h>
#include <eternal/core/graphics/vulkan/VulkanSwapChain.h>
#include <eternal/core/graphics/vulkan/VulkanGraphicsContext.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <filesystem>

namespace Eternal {

	constexpr uint32_t const MAX_FRAMES_IN_FLIGHT = 2;

	using VkStringArray = std::vector<const char*>;
	using VkString = const char*;
	using VkStringArrayPtr = const char**;

	class VulkanPlatform : public GraphicsPlatform , public VulkanGraphicsContext
	{

	public:

		struct PushConstants
		{
			glm::mat4 ViewProjection;
			glm::mat4 Transform;
		};

		struct Buffer
		{
			vk::Buffer handle = nullptr;
			vk::DeviceMemory memory = nullptr;
			vk::DeviceSize size = 0;
			vk::BufferUsageFlagBits usage = vk::BufferUsageFlagBits::eIndexBuffer;
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

		vk::RenderPass createRenderPass(const vk::Device& logicalDevice);

		vk::ShaderModule loadShader(const vk::Device& logicalDevice, const std::filesystem::path& path);

		bool validateExtensions(VkStringArray extensions);

		bool validateLayers(VkStringArray layers);

		bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray requestedExtensions);

		bool checkDeviceIsSuitable(vk::PhysicalDevice& device);

		void logDeviceProps(const vk::PhysicalDevice& device);

		void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

		vk::CommandBuffer getCommandBuffer() override { return m_CommandBuffers[m_CurrentFrame]; }

	private:

		void initializePipeline();

		void initializeRenderPass();

		void initializeFrameBuffers();

		void initializeCommandPool();

		void initializeCommandBuffer();

		void initializeSyncObjects();

		void initializeBuffers();

		void createOrResizeBuffer(Buffer& buffer, uint32_t newSize);

		uint32_t getMemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits);

		std::string m_ApplicationName;

		vk::SurfaceKHR m_Surface = nullptr;

		Buffer m_VertexBuffer;
		Buffer m_IndexBuffer;
		PushConstants m_PushConstants;

		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
		std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
		std::vector<vk::Fence> m_InFlightFences;

		uint32_t m_CurrentFrame = 0;

		glm::vec3 m_CubePosition = glm::vec3(0);
		glm::vec3 m_CubeRotation = glm::vec3(0);

		glm::vec3 m_CameraPosition = glm::vec3(0, 0, 3);
		glm::vec3 m_CameraRotation = glm::vec3(0);

		float m_CubeScale = 0.4f;

		Timer timer;
	};
}