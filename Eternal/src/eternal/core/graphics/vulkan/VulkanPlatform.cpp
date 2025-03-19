#include "VulkanPlatform.h"
#include "VulkanConstants.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <set>
#include <fstream>

namespace Eternal {
	VulkanPlatform::VulkanPlatform(std::string& applicationName, Eternal::Window* window) : m_ApplicationName(applicationName), m_Window(window)
	{
		initialize();
	}

	VulkanPlatform::~VulkanPlatform()
	{
		shutDown();
	}

	void VulkanPlatform::initialize()
	{
		m_VkInstance = createInstance(m_ApplicationName);

		m_PhysicalDevice = choosePhysicalDevice(m_VkInstance);

		m_GraphicsQueueFamilyIndex = identifyGraphicsQueueFamilyIndex(m_PhysicalDevice, vk::QueueFlagBits::eGraphics);
		if (m_GraphicsQueueFamilyIndex == INVALID_VK_INDEX)
		{
			Eternal::Logger::Error("Graphics Queue Family Index is Invalid");
			return;
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->getNativeWindow());

		VkSurfaceKHR surface = nullptr;
		glfwCreateWindowSurface(m_VkInstance, window, nullptr, &surface);
		m_Surface = surface;

		m_PresentQueueFamilyIndex = identifyPresentQueueFamilyIndex(m_PhysicalDevice, m_Surface);
		if (m_PresentQueueFamilyIndex == INVALID_VK_INDEX)
		{
			Eternal::Logger::Error("Present Queue Family Index is Invalid");
			return;
		}

		m_LogicalDevice = createLogicalDevice(m_PhysicalDevice, m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex);

		m_GraphicsQueue = m_LogicalDevice.getQueue(m_GraphicsQueueFamilyIndex, m_GraphicsQueueIndex);

		m_PresentQueue = m_LogicalDevice.getQueue(m_PresentQueueFamilyIndex, m_PresentQueueIndex);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D fallbackExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		m_VulkanSwapChain = new VulkanSwapChain(
			m_LogicalDevice,
			m_PhysicalDevice,
			m_PresentQueue,
			m_Surface,
			fallbackExtent,
			m_GraphicsQueueFamilyIndex,
			m_PresentQueueFamilyIndex
		);

		initializeRenderPass();

		initializeBuffers();

		initializePipeline();

		initializeFrameBuffers();

		initializeCommandPool();

		initializeCommandBuffer();

		initializeSyncObjects();

		m_VulkanGraphicsContext = new VulkanGraphicsContext(
			m_VkInstance, m_PhysicalDevice,
			m_LogicalDevice, m_GraphicsQueue,
			m_GraphicsQueueFamilyIndex
		);
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
			if (checkDeviceIsSuitable(device)) {
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

	void VulkanPlatform::initializeRenderPass()
	{
		m_RenderPass = createRenderPass(m_LogicalDevice);
	}

	void VulkanPlatform::initializePipeline()
	{
		std::vector<vk::PushConstantRange> pushConstantRanges;

		vk::PushConstantRange pushConstantRange = vk::PushConstantRange()
			.setOffset(0)
			.setSize(sizeof(glm::mat4) * 2)
			.setStageFlags(vk::ShaderStageFlagBits::eVertex);

		pushConstantRanges.push_back(pushConstantRange);

		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(0)
			.setPushConstantRanges(pushConstantRanges);

		m_PipelineLayout = m_LogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);

		vk::ShaderModule vertexShaderModule = loadShader(m_LogicalDevice, "src/eternal/core/graphics/shader/bin/vert.spv");
		vk::ShaderModule fragmentShaderModule = loadShader(m_LogicalDevice, "src/eternal/core/graphics/shader/bin/frag.spv");

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
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(VK_FALSE);

		vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(VK_FALSE)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

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

		std::vector<vk::VertexInputBindingDescription> bindingDescs;
		vk::VertexInputBindingDescription bindingDesc = vk::VertexInputBindingDescription()
			.setBinding(0)
			.setStride(sizeof(glm::vec3))
			.setInputRate(vk::VertexInputRate::eVertex);
		bindingDescs.push_back(bindingDesc);

		std::vector<vk::VertexInputAttributeDescription> attributeDescs;
		vk::VertexInputAttributeDescription attributeDesc = vk::VertexInputAttributeDescription()
			.setLocation(0)
			.setBinding(bindingDescs[0].binding)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(0);
		attributeDescs.push_back(attributeDesc);

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptions(bindingDescs)
			.setVertexAttributeDescriptions(attributeDescs);

		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
			.setStages(shaderStages)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssemblyInfo)
			.setPViewportState(&viewPortStateCreateInfo)
			.setPRasterizationState(&rasterizationStateCreateInfo)
			.setPMultisampleState(&multiSampleStateCreateInfo)
			.setPColorBlendState(&colorBlendStateCreateInfo)
			.setPDynamicState(&dynamicStateCreateInfo)
			.setLayout(m_PipelineLayout)
			.setRenderPass(m_RenderPass)
			.setSubpass(0);

		try
		{
			auto [result, graphicsPipeline] = m_LogicalDevice.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo, nullptr);
			if (result == vk::Result::eSuccess)
			{
				m_GraphicsPipeline = graphicsPipeline;
			}
		}
		catch (vk::SystemError err)
		{
			Eternal::Logger::Error("Graphics Pipeline Exception : {}", err.what());
		}

		m_LogicalDevice.destroyShaderModule(vertexShaderModule);
		m_LogicalDevice.destroyShaderModule(fragmentShaderModule);
	}

	void VulkanPlatform::initializeFrameBuffers()
	{
		std::vector<vk::ImageView> swapChainImageViews = m_VulkanSwapChain->getImageViews();
		VulkanSwapChain::SwapChainDetails swapChainDetails = m_VulkanSwapChain->getSwapChainDetails();

		m_SwapChainFrameBuffers.resize(swapChainImageViews.size());

		for (uint32_t i = 0; i < swapChainImageViews.size(); i++)
		{
			vk::ImageView attachment[] = { swapChainImageViews[i] };

			vk::FramebufferCreateInfo frameBufferInfo = vk::FramebufferCreateInfo()
				.setRenderPass(m_RenderPass)
				.setAttachments(attachment)
				.setWidth(swapChainDetails.extent.width)
				.setHeight(swapChainDetails.extent.height)
				.setLayers(1);

			m_SwapChainFrameBuffers[i] = m_LogicalDevice.createFramebuffer(frameBufferInfo);
		}
	}

	void VulkanPlatform::initializeCommandPool()
	{
		vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(m_GraphicsQueueIndex);

		m_CommandPool = m_LogicalDevice.createCommandPool(commandPoolCreateInfo);
	}

	void VulkanPlatform::initializeCommandBuffer()
	{
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(m_CommandPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

		m_CommandBuffers = m_LogicalDevice.allocateCommandBuffers(commandBufferAllocateInfo);
	}

	void VulkanPlatform::initializeSyncObjects()
	{
		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

			m_InFlightFences[i] = m_LogicalDevice.createFence(fenceCreateInfo);

			vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();
			m_ImageAvailableSemaphores[i] = m_LogicalDevice.createSemaphore(semaphoreCreateInfo);
			m_RenderFinishedSemaphores[i] = m_LogicalDevice.createSemaphore(semaphoreCreateInfo);
		}
	}

	void VulkanPlatform::initializeBuffers()
	{
		glm::vec3 vertexes[8] = {
			glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f,  0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f),
			glm::vec3(0.5f, -0.5f,  0.5f),

			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f)
		};

		uint32_t indices[36] = {
			0, 1, 2, 2, 3, 0, // Front
			3, 2, 5, 5, 4, 3, // Right
			4, 5, 6, 6, 7, 4, // Back
			7, 6, 1, 1, 0, 7, // Side
			1, 6, 5, 5, 2, 1, // Top
			7, 0, 3, 3, 4, 7, // Bottom
		};

		m_VertexBuffer.usage = vk::BufferUsageFlagBits::eVertexBuffer;
		createOrResizeBuffer(m_VertexBuffer, sizeof(vertexes));

		m_IndexBuffer.usage = vk::BufferUsageFlagBits::eIndexBuffer;
		createOrResizeBuffer(m_IndexBuffer, sizeof(indices));

		glm::vec3* vertexBufferMemory = static_cast<glm::vec3*>(m_LogicalDevice.mapMemory(m_VertexBuffer.memory, 0, sizeof(vertexes)));
		memcpy(vertexBufferMemory, vertexes, sizeof(vertexes));

		uint32_t* indexBufferMemory = static_cast<uint32_t*>(m_LogicalDevice.mapMemory(m_IndexBuffer.memory, 0, sizeof(indices)));
		memcpy(indexBufferMemory, indices, sizeof(indices));

		vk::MappedMemoryRange range[2] = {};

		range[0].setMemory(m_VertexBuffer.memory)
			.setSize(VK_WHOLE_SIZE);
		range[1].setMemory(m_IndexBuffer.memory)
			.setSize(VK_WHOLE_SIZE);

		m_LogicalDevice.flushMappedMemoryRanges(range);
		m_LogicalDevice.unmapMemory(m_VertexBuffer.memory);
		m_LogicalDevice.unmapMemory(m_IndexBuffer.memory);
	}

	void VulkanPlatform::createOrResizeBuffer(Buffer& buffer, uint32_t newSize)
	{
		if (buffer.handle)
			m_LogicalDevice.destroyBuffer(buffer.handle);

		if (buffer.memory)
			m_LogicalDevice.freeMemory(buffer.memory);

		vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
			.setSize(newSize)
			.setUsage(buffer.usage)
			.setSharingMode(vk::SharingMode::eExclusive);

		buffer.handle = m_LogicalDevice.createBuffer(bufferCreateInfo);

		vk::MemoryRequirements memoryRequirements = m_LogicalDevice.getBufferMemoryRequirements(buffer.handle);

		vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memoryRequirements.size)
			.setMemoryTypeIndex(getMemoryType(vk::MemoryPropertyFlagBits::eHostVisible, memoryRequirements.memoryTypeBits));

		buffer.memory = m_LogicalDevice.allocateMemory(memoryAllocateInfo);

		m_LogicalDevice.bindBufferMemory(buffer.handle, buffer.memory, 0);

		buffer.size = memoryRequirements.size;
	}

	uint32_t VulkanPlatform::getMemoryType(vk::MemoryPropertyFlags properties, uint32_t type_bits)
	{
		vk::PhysicalDeviceMemoryProperties prop = m_PhysicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
				return i;
		return 0xFFFFFFFF;
	}

	void VulkanPlatform::shutDown()
	{
		m_LogicalDevice.waitIdle();

		for (auto frameBuffer : m_SwapChainFrameBuffers)
		{
			m_LogicalDevice.destroyFramebuffer(frameBuffer);
		}

		m_VulkanSwapChain->destroy();

		if (m_VertexBuffer.handle)
		{
			m_LogicalDevice.destroyBuffer(m_VertexBuffer.handle);
		}

		if (m_VertexBuffer.memory)
		{
			m_LogicalDevice.freeMemory(m_VertexBuffer.memory);
		}

		if (m_IndexBuffer.handle)
		{
			m_LogicalDevice.destroyBuffer(m_IndexBuffer.handle);
		}

		if (m_IndexBuffer.memory)
		{
			m_LogicalDevice.freeMemory(m_IndexBuffer.memory);
		}

		for (auto semaphore : m_ImageAvailableSemaphores)
		{
			m_LogicalDevice.destroySemaphore(semaphore);
		}

		for (auto semaphore : m_RenderFinishedSemaphores)
		{
			m_LogicalDevice.destroySemaphore(semaphore);
		}

		for (auto fence : m_InFlightFences)
		{
			m_LogicalDevice.destroyFence(fence);
		}

		m_LogicalDevice.destroyCommandPool(m_CommandPool);
		m_LogicalDevice.destroyPipeline(m_GraphicsPipeline);
		m_LogicalDevice.destroyPipelineLayout(m_PipelineLayout);
		m_LogicalDevice.destroyRenderPass(m_RenderPass);
		m_LogicalDevice.destroy();

		delete m_VulkanSwapChain;

		m_VkInstance.destroySurfaceKHR(m_Surface);
		m_VkInstance.destroy();

		delete m_VulkanGraphicsContext;
	}

	void VulkanPlatform::render()
	{
		vk::Result result;

		uint32_t imageIndex;
		result = m_VulkanSwapChain->acquire(m_ImageAvailableSemaphores[m_CurrentFrame], &imageIndex);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
			m_VulkanSwapChain->recreate();
			return;
		}
		else if (result != vk::Result::eSuccess) {
			Logger::Error("Failed to acquire swapchain image");
			return;
		}

		result = m_LogicalDevice.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT16_MAX);
		if (result != vk::Result::eSuccess)
		{
			Eternal::Logger::Info("Failed to wait for fence");
			return;
		}

		result = m_LogicalDevice.resetFences(1, &m_InFlightFences[m_CurrentFrame]);

		if (result != vk::Result::eSuccess)
		{
			Eternal::Logger::Info("Failed to rest for fence");
			return;
		}

		vk::CommandBuffer& commandBuffer = m_CommandBuffers[m_CurrentFrame];
		commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
		recordCommandBuffer(commandBuffer, imageIndex);

		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&m_ImageAvailableSemaphores[m_CurrentFrame])
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(1)
			.setPCommandBuffers(&commandBuffer)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&m_RenderFinishedSemaphores[m_CurrentFrame]);

		result = m_GraphicsQueue.submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
		if (result != vk::Result::eSuccess)
		{
			Eternal::Logger::Info("Failed to submit Command Buffer");
			return;
		}

		m_VulkanSwapChain->present(m_RenderFinishedSemaphores[m_CurrentFrame], imageIndex);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
			m_VulkanSwapChain->recreate();
		}
		else if (result != vk::Result::eSuccess)
		{
			Logger::Error("Failed to present swapchain image");
			return;
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

	vk::RenderPass VulkanPlatform::createRenderPass(const vk::Device& logicalDevice)
	{

		VulkanSwapChain::SwapChainDetails swapChainDetails = m_VulkanSwapChain->getSwapChainDetails();

		vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
			.setFormat(swapChainDetails.surfaceFormat.format)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		vk::AttachmentReference colorAttachmentReference = vk::AttachmentReference()
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::SubpassDescription subPass = vk::SubpassDescription()
			.setColorAttachmentCount(1)
			.setPColorAttachments(&colorAttachmentReference);

		vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo()
			.setAttachmentCount(1)
			.setPAttachments(&colorAttachment)
			.setSubpassCount(1)
			.setPSubpasses(&subPass);

		return  logicalDevice.createRenderPass(renderPassCreateInfo);
	}

	void VulkanPlatform::recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VulkanSwapChain::SwapChainDetails swapChainDetails = m_VulkanSwapChain->getSwapChainDetails();

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, -2.0f);

		float deltaTime = timer.getDeltaTime();
		float rotationSpeed = 45.0f;
		m_CubeRotation.y += rotationSpeed * deltaTime;

		vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();
		commandBuffer.begin(commandBufferBeginInfo);

		vk::Rect2D renderArea = vk::Rect2D()
			.setOffset({ 0,0 })
			.setExtent(swapChainDetails.extent);

		vk::ClearColorValue clearColor = vk::ClearColorValue(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f});
		vk::ClearValue clearValue = vk::ClearValue(clearColor);

		vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(m_RenderPass)
			.setFramebuffer(m_SwapChainFrameBuffers[imageIndex])
			.setRenderArea(renderArea)
			.setClearValueCount(1)
			.setPClearValues(&clearValue);

		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline);

		glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), m_CameraPosition)
			* glm::eulerAngleXYZ(glm::radians(m_CameraRotation.x), glm::radians(m_CameraRotation.y), glm::radians(m_CameraRotation.z));

		float aspectRatio = (float)m_Window->getWidth() / (float)m_Window->getHeight();
		m_PushConstants.ViewProjection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f)
			* glm::inverse(cameraTransform);

		m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), translation)
			* glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x), glm::radians(m_CubeRotation.y), glm::radians(m_CubeRotation.z));

		commandBuffer.pushConstants(m_PipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstants), &m_PushConstants);

		vk::DeviceSize offset = vk::DeviceSize(0);

		commandBuffer.bindVertexBuffers(0, 1, &m_VertexBuffer.handle, &offset);

		commandBuffer.bindIndexBuffer(m_IndexBuffer.handle, offset, vk::IndexType::eUint32);

		vk::Viewport viewport = vk::Viewport()
			.setX(0.0f)
			.setY(0.0f)
			.setWidth((float)swapChainDetails.extent.width)
			.setHeight((float)swapChainDetails.extent.height)
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f);

		commandBuffer.setViewport(0, 1, &viewport);

		vk::Rect2D scissor = vk::Rect2D()
			.setOffset({ 0,0 })
			.setExtent(swapChainDetails.extent);

		commandBuffer.setScissor(0, 1, &scissor);

		commandBuffer.drawIndexed(36, 1, 0, 0, 0);

		commandBuffer.endRenderPass();

		commandBuffer.end();
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

		Eternal::Logger::Debug("*********** Requested Extensions ***********");
		for (const char* extension : requestedExtensions) {
			Eternal::Logger::Debug("{}", extension);
		}

		bool isExtensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions);
		return isExtensionsSupported && device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
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
