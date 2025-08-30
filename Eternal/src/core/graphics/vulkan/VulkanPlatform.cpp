#include "VulkanPlatform.h"
#include "VulkanConstants.h"

#include <core/resource/ResourceManager.h>
#include <core/resource/ShaderProgram.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <set>
#include <fstream>

namespace Eternal {

	VulkanPlatform::VulkanPlatform(const Builder& builder) {
		m_ApplicationName = builder->applicationName;

		VulkanPlatform::initialize();
	}

	VulkanPlatform::~VulkanPlatform() {
		VulkanPlatform::shutDown();
	}

	void VulkanPlatform::initialize() {

		m_VkInstance = createInstance(m_ApplicationName);
		m_PhysicalDevice = choosePhysicalDevice(m_VkInstance);
	}

	vk::Instance VulkanPlatform::createInstance(const std::string& applicationName) {
		uint32_t version = 0;
		vkEnumerateInstanceVersion(&version);

		Eternal::Logger::Info("Vulkan Variant {}", VK_API_VERSION_VARIANT(version));
		Eternal::Logger::Info("Vulkan Major {}", VK_API_VERSION_MAJOR(version));
		Eternal::Logger::Info("Vulkan Minor {}", VK_API_VERSION_MINOR(version));
		Eternal::Logger::Info("Vulkan Patch {}", VK_API_VERSION_PATCH(version));

		// Removing Patch 
		version &= ~(0xFFFU);

		vk::ApplicationInfo applicationInfo = vk::ApplicationInfo()
			.setPApplicationName(applicationName.c_str())
			.setPEngineName("Eternal")
			.setApiVersion(version)
			.setEngineVersion(version)
			.setApplicationVersion(version);

		uint32_t extensionCount = 0;
		VkStringArrayPtr glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

		if (glfwExtensions == nullptr) {
			Eternal::Logger::Error("vulkan glfwExtensions are null , probably before call glfwInit() function before createInstance()");
			return nullptr;
		}

		VkStringArray extensions(glfwExtensions, glfwExtensions + extensionCount);

		for (VkString extensionName : extensions)
			Eternal::Logger::Info("extension = {}", extensionName);

		VkStringArray layers;
#if ETERNAL_FLAG_ENABLED(ETERNAL_VULKAN_DEBUG_VALIDATION)
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif 

		if (!validateExtensions(extensions)) {
			Eternal::Logger::Error("validation failed for extensions");
			return nullptr;
		}

		if (!validateLayers(layers)) {
			Eternal::Logger::Error("validation failed for layer");
			return nullptr;
		}

		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo()
			.setFlags(vk::InstanceCreateFlags())
			.setPApplicationInfo(&applicationInfo)
			.setPEnabledLayerNames(layers)
			.setPEnabledExtensionNames(extensions);

		return vk::createInstance(createInfo, nullptr);
	}

	vk::PhysicalDevice VulkanPlatform::choosePhysicalDevice(vk::Instance& instance) {
		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

		for (vk::PhysicalDevice& device : availableDevices) {
			if (checkDeviceIsSuitable(device) && device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
				Eternal::Logger::Info("************ Supported Device Properties ************");
				logDeviceProps(device);
				return device;
			}
		}

		return nullptr;
	}

	vk::Device VulkanPlatform::createLogicalDevice(vk::PhysicalDevice& device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex) {
		vk::Device logicalDevice;
		float queuePriority = 1.0f;

		VkStringArray deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::vector<uint32_t> uniqueQueueFamilyIndices;
		uniqueQueueFamilyIndices.push_back(graphicsQueueFamilyIndex);
		if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
			uniqueQueueFamilyIndices.push_back(presentQueueFamilyIndex);
		}

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
			vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
				.setFlags(vk::DeviceQueueCreateFlags())
				.setQueueFamilyIndex(queueFamilyIndex)
				.setQueueCount(1)
				.setPQueuePriorities(&queuePriority);

			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeature = vk::PhysicalDeviceFeatures();
		deviceFeature.setSamplerAnisotropy(true);

		vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
			.setFlags(vk::DeviceCreateFlags())
			.setPEnabledExtensionNames(deviceExtensions)
			.setQueueCreateInfos(queueCreateInfos)
			.setPEnabledFeatures(&deviceFeature);

		logicalDevice = device.createDevice(deviceCreateInfo);

		return logicalDevice;
	}

	void VulkanPlatform::shutDown() {
		if (m_Surface) {
			m_VkInstance.destroySurfaceKHR(m_Surface);
		}

		m_VkInstance.destroy();
	}

	SwapChain* VulkanPlatform::createSwapChain(Window* window) {
		VulkanWindow* vkWindow = dynamic_cast<VulkanWindow*>(window);

		if (!vkWindow) {
			Eternal::Logger::Debug("Returning Null SwapChain, expecting VulkanWindow in createSwapChain(..)");
			return nullptr;
		}

		m_Surface = vkWindow->createWindowSurface(m_VkInstance);

		vk::Extent2D fallbackExtent = vkWindow->getExtent();

		m_PresentQueueFamilyIndex = identifyPresentQueueFamilyIndex(m_PhysicalDevice, m_Surface);
		ETERNAL_ASSERT(m_PresentQueueFamilyIndex != INVALID_VK_INDEX, "Present Queue Family Index is Invalid");

		m_GraphicsQueueFamilyIndex = identifyGraphicsQueueFamilyIndex(m_PhysicalDevice, vk::QueueFlagBits::eGraphics);
		ETERNAL_ASSERT(m_GraphicsQueueFamilyIndex != INVALID_VK_INDEX, "Graphics Queue Family Index is Invalid");

		m_LogicalDevice = createLogicalDevice(m_PhysicalDevice, m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex);

		m_PresentQueue = m_LogicalDevice.getQueue(m_PresentQueueFamilyIndex, m_PresentQueueIndex);

		m_GraphicsQueue = m_LogicalDevice.getQueue(m_GraphicsQueueFamilyIndex, m_GraphicsQueueIndex);

		return Memory::Allocate<VulkanSwapChain>(m_VkInstance, m_LogicalDevice, m_PhysicalDevice, m_PresentQueue, m_Surface, fallbackExtent, m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex);
	}

	vk::ShaderModule VulkanPlatform::loadShader(const vk::Device& logicalDevice, const std::filesystem::path& path) {
		ShaderProgram* shader = ResourceManager::get().loadResource<ShaderProgram>(path.string());

		vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(shader->getBlobSize())
			.setPCode(shader->getBlob());

		vk::ShaderModule shaderModule;
		shaderModule = logicalDevice.createShaderModule(shaderModuleCreateInfo);
		return shaderModule;
	}

	uint32_t VulkanPlatform::identifyGraphicsQueueFamilyIndex(vk::PhysicalDevice& device, vk::QueueFlags flags) {
		uint32_t graphicsQueueFamilyIndex = INVALID_VK_INDEX;
		std::vector<vk::QueueFamilyProperties> queueFamiliesProperties = device.getQueueFamilyProperties();
		for (uint32_t i = 0; i < queueFamiliesProperties.size(); i++) {
			vk::QueueFamilyProperties props = queueFamiliesProperties[i];
			if (props.queueCount != 0 && props.queueFlags & flags) {
				graphicsQueueFamilyIndex = i;
				break;
			}
		}
		return graphicsQueueFamilyIndex;
	}

	uint32_t VulkanPlatform::identifyPresentQueueFamilyIndex(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
		uint32_t presentQueueFamilyIndex = INVALID_VK_INDEX;
		std::vector<vk::QueueFamilyProperties> queueFamiliesProperties = device.getQueueFamilyProperties();
		for (uint32_t i = 0; i < queueFamiliesProperties.size(); i++) {
			if (device.getSurfaceSupportKHR(i, surface)) {
				presentQueueFamilyIndex = i;
				break;
			}
		}
		return presentQueueFamilyIndex;
	}

	uint32_t VulkanPlatform::getMemoryType(vk::PhysicalDevice physicalDevice, vk::MemoryPropertyFlags properties, uint32_t typeBits) {
		vk::PhysicalDeviceMemoryProperties prop = physicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			if ((prop.memoryTypes[i].propertyFlags & properties) == properties && typeBits & (1 << i))
				return i;
		return 0xFFFFFFFF;
	}

	bool VulkanPlatform::validateExtensions(VkStringArray extensions)
	{
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		for (VkString extension : extensions) {
			auto it = std::find_if(supportedExtensions.begin(), supportedExtensions.end(),
				[&](const vk::ExtensionProperties& supportedExtension) {
					return strcmp(extension, supportedExtension.extensionName) == 0;
				});

			if (it != supportedExtensions.end()) {
				Eternal::Logger::Debug("{} Extension Supported", extension);
			}
			else {
				Eternal::Logger::Error("{} Extension Not Supported", extension);
				return false;
			}
		}

		return true;
	}

	bool VulkanPlatform::validateLayers(VkStringArray layers) {
		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

		for (VkString layer : layers) {
			auto it = std::find_if(supportedLayers.begin(), supportedLayers.end(),
				[&](const vk::LayerProperties& supportedLayer) {
					return strcmp(layer, supportedLayer.layerName) == 0;
				});

			if (it != supportedLayers.end()) {
				Eternal::Logger::Debug("{} Layer Supported", layer);
			}
			else {
				Eternal::Logger::Error("{} Layer Not Supported", layer);
				return false;
			}
		}

		return true;
	}

	bool VulkanPlatform::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray requestedExtensions) {
		std::set<std::string> requiredExtesnions(requestedExtensions.begin(), requestedExtensions.end());

		for (vk::ExtensionProperties& extensionProperty : device.enumerateDeviceExtensionProperties()) {
			requiredExtesnions.erase(extensionProperty.extensionName);
		}

		return requiredExtesnions.empty();
	}

	bool VulkanPlatform::checkDeviceIsSuitable(const vk::PhysicalDevice& device) {
		VkStringArray requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool isExtensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions);
		return isExtensionsSupported;
	}

	void VulkanPlatform::logDeviceProps(const vk::PhysicalDevice& device) {
		vk::PhysicalDeviceProperties properties = device.getProperties();

		Eternal::Logger::Debug("Device Name : {}", properties.deviceName.data());

		std::string deviceType = "";
		switch (properties.deviceType) {

		case (vk::PhysicalDeviceType::eCpu):
			deviceType = "CPU";
			break;

		case (vk::PhysicalDeviceType::eDiscreteGpu):
			deviceType = "Discrete GPU";
			break;

		case (vk::PhysicalDeviceType::eIntegratedGpu):
			deviceType = "Integrated GPU";
			break;

		case (vk::PhysicalDeviceType::eVirtualGpu):
			deviceType = "Virtual GPU";
			break;

		default:
			deviceType = "Other";
			break;
		}
		Eternal::Logger::Debug("Device Type : {}", deviceType);
	}

	vk::CommandPool VulkanPlatform::createCommandPool(vk::CommandPoolCreateFlags commandPoolCreateFlagBits) {
		vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(m_GraphicsQueueIndex);

		return m_LogicalDevice.createCommandPool(commandPoolCreateInfo);
	}

	std::vector<vk::CommandBuffer> VulkanPlatform::allocateCommandBuffers(vk::CommandPool commandPool, vk::CommandBufferLevel level, uint32_t count) {
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(commandPool)
			.setLevel(level)
			.setCommandBufferCount(count);

		return m_LogicalDevice.allocateCommandBuffers(commandBufferAllocateInfo);
	}

	vk::CommandBuffer VulkanPlatform::allocateCommandBuffer(vk::CommandPool commandPool, vk::CommandBufferLevel level) {
		auto commandBuffer = allocateCommandBuffers(commandPool, level, 1);
		return commandBuffer.front();
	}

	void VulkanPlatform::destroyCommandPool(vk::CommandPool commandPool) {
		m_LogicalDevice.destroyCommandPool(commandPool);
	}

	vk::CommandBuffer VulkanPlatform::beginSingleCommand(vk::CommandPool commandPool) {
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(commandPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);

		vk::CommandBuffer commandBuffer = m_LogicalDevice.allocateCommandBuffers(commandBufferAllocateInfo)[0];

		vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
			.setPInheritanceInfo(nullptr);

		commandBuffer.begin(beginInfo);
		return commandBuffer;
	}

	void VulkanPlatform::endSingleCommand(vk::CommandPool commandPool, vk::CommandBuffer commandBuffer, vk::Queue queue) {
		commandBuffer.end();
		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBuffers(commandBuffer);

		queue.submit(submitInfo, nullptr);
		queue.waitIdle();
		m_LogicalDevice.freeCommandBuffers(commandPool, commandBuffer);
	}

	void VulkanPlatform::executeOneCommand(vk::CommandPool commandPool, vk::Queue queue, const std::function<void(vk::CommandBuffer)>& function) {
		vk::CommandBuffer commandBuffer = beginSingleCommand(commandPool);
		function(commandBuffer);
		endSingleCommand(commandPool, commandBuffer, queue);
	}
}
