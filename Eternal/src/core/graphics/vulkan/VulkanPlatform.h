#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <core/graphics/GraphicsPlatform.h>
#include <utils/Base.h>
#include <core/graphics/vulkan/VulkanWindow.h>
#include <core/graphics/Timer.h>
#include <core/graphics/vulkan/VulkanSwapChain.h>
#include <core/graphics/vulkan/VulkanGraphicsContext.h>
#include <core/graphics/Vertex.h>
#include <core/graphics/vulkan/VulkanBufferManager.h>
#include <core/graphics/Camera.hpp>
#include <core/graphics/vulkan/VulkanDescsriptorSetLayout.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <filesystem>

namespace Eternal {

	using VkStringArray = std::vector<const char*>;
	using VkString = const char*;
	using VkStringArrayPtr = const char**;

	class VulkanPlatform : public GraphicsPlatform, public VulkanGraphicsContext{
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
		bool checkDeviceIsSuitable(const vk::PhysicalDevice& device);
		void logDeviceProps(const vk::PhysicalDevice& device);

		static uint32_t getMemoryType(vk::PhysicalDevice physicalDevice, vk::MemoryPropertyFlags properties, uint32_t typeBits);

	private:
		std::string m_ApplicationName;
		Timer timer;
	};
}