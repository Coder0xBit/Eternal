#include "VulkanImGuiLayer.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/backends/imgui_impl_glfw.h"

namespace Eternal {

	VulkanImGuiLayer::VulkanImGuiLayer(VulkanPlatform* vulkanPlatform, VulkanSwapChain* swapChain, Window* window)
		: m_VulkanPlatform(vulkanPlatform),
		m_SwapChain(swapChain), m_Window(window)
	{
		init();
	}

	void VulkanImGuiLayer::init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = m_VulkanPlatform->getVkInstance();
		initInfo.PhysicalDevice = m_VulkanPlatform->getPhysicalDevice();
		initInfo.Device = m_VulkanPlatform->getLogicalDevice();
		initInfo.Queue = m_VulkanPlatform->getGraphicsQueue();
		initInfo.RenderPass = m_SwapChain->getRenderPass();
		initInfo.DescriptorPoolSize = 2;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = 2;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		ImGui_ImplVulkan_Init(&initInfo);

		GLFWwindow* nativeWindow = static_cast<GLFWwindow*>(m_Window->getNativeWindow());
		ImGui_ImplGlfw_InitForVulkan(nativeWindow, true);
		ImGui_ImplVulkan_CreateFontsTexture();
	}

	VulkanImGuiLayer::~VulkanImGuiLayer()
	{
		vk::Device device = m_VulkanPlatform->getLogicalDevice();
		device.waitIdle();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImGuiLayer::beginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::render(FrameInfo* frameInfo)
	{
		ImGui::Render();
		auto commandBuffer = static_cast<VulkanFrameInfo*>(frameInfo)->commandBuffer;
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}
}

