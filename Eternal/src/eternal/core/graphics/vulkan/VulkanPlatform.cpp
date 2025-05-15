#include "VulkanPlatform.h"
#include "VulkanConstants.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <set>
#include <fstream>

namespace Eternal {

	VulkanPlatform::VulkanPlatform(const Builder& builder)
	{
		m_ApplicationName = builder->applicationName;

		initialize();
	}

	VulkanPlatform::~VulkanPlatform()
	{
		shutDown();
	}

	void VulkanPlatform::initialize()
	{
		m_VertexShaderPath = "src/eternal/core/graphics/shader/bin/vert.spv";

		m_FragmentShaderPath = "src/eternal/core/graphics/shader/bin/frag.spv";

		m_VkInstance = createInstance(m_ApplicationName);

		m_PhysicalDevice = choosePhysicalDevice(m_VkInstance);
	}

	vk::Instance VulkanPlatform::createInstance(const std::string& applicationName)
	{
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

		if (glfwExtensions == nullptr)
		{
			Eternal::Logger::Error("vulkan glfwExtensions are null , probably before call glfwInit() function before createInstance()");
			return nullptr;
		}

		VkStringArray extensions(glfwExtensions, glfwExtensions + extensionCount);

		for (VkString extensionName : extensions)
			Eternal::Logger::Info("extension = {}", extensionName);

		VkStringArray layers;
#if ETERNAL_VULKAN_ENABLED(ETERNAL_VULKAN_DEBUG_VALIDATION)
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif 

		if (!validateExtensions(extensions))
		{
			Eternal::Logger::Error("validation failed for extensions");
			return nullptr;
		}

		if (!validateLayers(layers))
		{
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

	vk::PhysicalDevice VulkanPlatform::choosePhysicalDevice(vk::Instance& instance)
	{
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

	vk::Device VulkanPlatform::createLogicalDevice(vk::PhysicalDevice& device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex)
	{
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

		vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
			.setFlags(vk::DeviceCreateFlags())
			.setPEnabledExtensionNames(deviceExtensions)
			.setQueueCreateInfos(queueCreateInfos)
			.setPEnabledFeatures(&deviceFeature);

		logicalDevice = device.createDevice(deviceCreateInfo);

		return logicalDevice;
	}

	void VulkanPlatform::shutDown()
	{
		if (m_Surface)
		{
			m_VkInstance.destroySurfaceKHR(m_Surface);
		}

		m_VkInstance.destroy();
	}

	SwapChain* VulkanPlatform::createSwapChain(Window* window)
	{
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

	vk::ShaderModule VulkanPlatform::loadShader(const vk::Device& logicalDevice, const std::filesystem::path& path)
	{
		std::ifstream stream(path, std::ios::binary);
		if (!stream) {
			Eternal::Logger::Error("Failed load stream in loadShader(..)");
			return nullptr;
		}

		stream.seekg(0, std::ios_base::end);
		std::streampos size = stream.tellg();
		stream.seekg(0, std::ios_base::beg);

		std::vector<char> buffer(size);
		stream.read(buffer.data(), size);
		stream.close();

		vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(buffer.size())
			.setPCode(reinterpret_cast<uint32_t*>(buffer.data()));

		vk::ShaderModule shaderModule;
		shaderModule = logicalDevice.createShaderModule(shaderModuleCreateInfo);
		return shaderModule;
	}

	uint32_t VulkanPlatform::identifyGraphicsQueueFamilyIndex(vk::PhysicalDevice& device, vk::QueueFlags flags)
	{
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

	uint32_t VulkanPlatform::identifyPresentQueueFamilyIndex(vk::PhysicalDevice& device, vk::SurfaceKHR& surface)
	{
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

	vk::PipelineLayout VulkanPlatform::createPipelineLayout(vk::PushConstantRange pushConstantRange)
	{
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(0)
			.setPushConstantRanges(pushConstantRange);

		vk::PipelineLayout pipelineLayout = m_LogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
		return pipelineLayout;
	}

	vk::Pipeline VulkanPlatform::createPipeline(vk::PipelineLayout pipelineLayout, vk::RenderPass renderPass)
	{
		vk::ShaderModule vertexShaderModule = loadShader(m_LogicalDevice, m_VertexShaderPath);
		vk::ShaderModule fragmentShaderModule = loadShader(m_LogicalDevice, m_FragmentShaderPath);

		vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(vertexShaderModule)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(fragmentShaderModule)
			.setPName("main");

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
			vertexShaderStageCreateInfo,
			fragmentShaderStageCreateInfo
		};

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(VK_FALSE);

		vk::PipelineViewportStateCreateInfo viewPortStateCreateInfo = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setScissorCount(1);

		vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eClockwise)
			.setDepthBiasEnable(VK_TRUE);

		vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(VK_FALSE)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		vk::PipelineDepthStencilStateCreateInfo depthStencil = vk::PipelineDepthStencilStateCreateInfo()
			.setDepthTestEnable(VK_TRUE)
			.setDepthWriteEnable(VK_TRUE)
			.setDepthCompareOp(vk::CompareOp::eLess)
			.setDepthBoundsTestEnable(VK_FALSE)
			.setStencilTestEnable(VK_FALSE);

		vk::PipelineColorBlendAttachmentState colorBlendAttachmentstate = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(VK_FALSE);

		vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(VK_FALSE)
			.setLogicOp(vk::LogicOp::eCopy)
			.setAttachmentCount(1)
			.setPAttachments(&colorBlendAttachmentstate)
			.setBlendConstants({ 0.0f,0.0f,0.0f,0.0f });

		std::vector<vk::DynamicState> dynamicState = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
			.setDynamicStates(dynamicState);

		auto bindingDescs = Eternal::Vertex::getBindingDescription();
		auto attributeDescs = Eternal::Vertex::getAttributeDescription();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptions(bindingDescs)
			.setVertexAttributeDescriptions(attributeDescs);

		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
			.setStages(shaderStages)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssemblyInfo)
			.setPViewportState(&viewPortStateCreateInfo)
			.setPDepthStencilState(&depthStencil)
			.setPRasterizationState(&rasterizationStateCreateInfo)
			.setPMultisampleState(&multiSampleStateCreateInfo)
			.setPColorBlendState(&colorBlendStateCreateInfo)
			.setPDynamicState(&dynamicStateCreateInfo)
			.setLayout(pipelineLayout)
			.setRenderPass(renderPass)
			.setSubpass(0);

		vk::Pipeline pipeline = nullptr;
		try
		{
			auto [result, graphicsPipeline] = m_LogicalDevice.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo, nullptr);
			if (result == vk::Result::eSuccess)
			{
				pipeline = graphicsPipeline;
			}
		}
		catch (vk::SystemError err)
		{
			Eternal::Logger::Error("Graphics Pipeline Exception : {}", err.what());
		}

		m_LogicalDevice.destroyShaderModule(vertexShaderModule);
		m_LogicalDevice.destroyShaderModule(fragmentShaderModule);

		return pipeline;
	}

	bool VulkanPlatform::validateExtensions(VkStringArray extensions)
	{
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		for (VkString extension : extensions) {
			auto it = std::find_if(supportedExtensions.begin(), supportedExtensions.end(),
				[&](const vk::ExtensionProperties& supportedExtension) {
					return strcmp(extension, supportedExtension.extensionName) == 0;
				});

			if (it != supportedExtensions.end())
			{
				Eternal::Logger::Debug("{} Extension Supported", extension);
			}
			else
			{
				Eternal::Logger::Error("{} Extension Not Supported", extension);
				return false;
			}
		}

		return true;
	}

	bool VulkanPlatform::validateLayers(VkStringArray layers)
	{
		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

		for (VkString layer : layers) {
			auto it = std::find_if(supportedLayers.begin(), supportedLayers.end(),
				[&](const vk::LayerProperties& supportedLayer) {
					return strcmp(layer, supportedLayer.layerName) == 0;
				});

			if (it != supportedLayers.end())
			{
				Eternal::Logger::Debug("{} Layer Supported", layer);
			}
			else
			{
				Eternal::Logger::Error("{} Layer Not Supported", layer);
				return false;
			}
		}

		return true;
	}

	bool VulkanPlatform::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const VkStringArray requestedExtensions)
	{
		std::set<std::string> requiredExtesnions(requestedExtensions.begin(), requestedExtensions.end());

		for (vk::ExtensionProperties& extensionProperty : device.enumerateDeviceExtensionProperties()) {
			requiredExtesnions.erase(extensionProperty.extensionName);
		}

		return requiredExtesnions.empty();
	}

	bool VulkanPlatform::checkDeviceIsSuitable(vk::PhysicalDevice& device)
	{
		VkStringArray requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool isExtensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions);
		return isExtensionsSupported;
	}

	void VulkanPlatform::logDeviceProps(const vk::PhysicalDevice& device)
	{
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
}
