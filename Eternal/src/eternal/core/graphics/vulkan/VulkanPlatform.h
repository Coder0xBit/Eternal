#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <eternal/core/graphics/GraphicsPlatform.h>
#include <eternal/utils/Base.h>
#include <eternal/core/graphics/vulkan/VulkanWindow.h>
#include <eternal/core/graphics/Timer.h>
#include <eternal/core/graphics/vulkan/VulkanSwapChain.h>
#include <eternal/core/graphics/vulkan/VulkanGraphicsContext.h>
#include <eternal/core/graphics/Vertex.h>
#include <eternal/core/graphics/vulkan/VulkanBufferManager.h>
#include <eternal/core/graphics/Camera.hpp>

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

	class VulkanPlatform : public GraphicsPlatform, public VulkanGraphicsContext
	{
	public:

		struct PushConstants {
			glm::mat4 transform{ 1.f };
		};

		VulkanPlatform(const Builder& builder);

		~VulkanPlatform();

		void initialize() override;

		void shutDown() override;

		SwapChain* createSwapChain(Window* window) override;

		vk::Instance createInstance(const std::string& applicationName);

		vk::PhysicalDevice choosePhysicalDevice(vk::Instance& instance);

		uint32_t identifyGraphicsQueueFamilyIndex(vk::PhysicalDevice& device, vk::QueueFlags flags);

		uint32_t identifyPresentQueueFamilyIndex(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);

		vk::Device createLogicalDevice(vk::PhysicalDevice& device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex);

		vk::ShaderModule loadShader(const vk::Device& logicalDevice, const std::filesystem::path& path);

		bool validateExtensions(VkStringArray extensions);

		bool validateLayers(VkStringArray layers);

		bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray requestedExtensions);

		bool checkDeviceIsSuitable(vk::PhysicalDevice& device);

		void logDeviceProps(const vk::PhysicalDevice& device);

		vk::PipelineLayout createPipelineLayout(vk::PushConstantRange pushConstantRange);

		vk::Pipeline createPipeline(vk::PipelineLayout pipelineLayout, vk::RenderPass renderPass);

	private:

		std::string m_ApplicationName;

		std::string m_VertexShaderPath;

		std::string m_FragmentShaderPath;

		Timer timer;
	};
}