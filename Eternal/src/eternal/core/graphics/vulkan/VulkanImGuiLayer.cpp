#include "VulkanImGuiLayer.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/backends/imgui_impl_glfw.h"

namespace Eternal {

	VulkanImGuiLayer::VulkanImGuiLayer(VulkanGraphicsContext* vulkanGraphicsContext)
		: m_VulkanGraphicsContext(vulkanGraphicsContext)
	{
		init();
	}

	void VulkanImGuiLayer::init()
	{
		vk::Device device = m_VulkanGraphicsContext->getLogicalDevice();

		std::vector<vk::DescriptorPoolSize> poolSizes = {
			{ vk::DescriptorType::eSampler, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
			{ vk::DescriptorType::eSampledImage, 1000 },
			{ vk::DescriptorType::eStorageImage, 1000 },
			{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
			{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eStorageBuffer, 1000 },
			{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
			{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
			{ vk::DescriptorType::eInputAttachment, 1000 }
		};

		vk::DescriptorPoolCreateInfo poolInfo{};
		poolInfo.maxSets = 1000;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		m_DescriptorPool = device.createDescriptorPool(poolInfo);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = m_VulkanGraphicsContext->getVkInstance();
		initInfo.PhysicalDevice = m_VulkanGraphicsContext->getPhysicalDevice();
		initInfo.Device = m_VulkanGraphicsContext->getLogicalDevice();
		initInfo.Queue = m_VulkanGraphicsContext->getGraphicsQueue();
		initInfo.RenderPass = m_VulkanGraphicsContext->getRenderPass();
		initInfo.DescriptorPool = m_DescriptorPool;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = 2;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		ImGui_ImplVulkan_Init(&initInfo);

		Eternal::Window* window = m_VulkanGraphicsContext->getWindow();
		GLFWwindow* nativeWindow = static_cast<GLFWwindow*>(window->getNativeWindow());

		ImGui_ImplGlfw_InitForVulkan(nativeWindow, false);
		ImGui_ImplVulkan_CreateFontsTexture();
		device.waitIdle();
	}

	VulkanImGuiLayer::~VulkanImGuiLayer()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		m_VulkanGraphicsContext->getLogicalDevice().destroyDescriptorPool(m_DescriptorPool);
	}

	void VulkanImGuiLayer::beginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::endFrame()
	{
		ImGui::Render();
		auto commandBuffer = m_VulkanGraphicsContext->getCommandBuffer();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}
}

