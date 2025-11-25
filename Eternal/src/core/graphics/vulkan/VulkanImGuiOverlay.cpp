#include "core/graphics/vulkan/VulkanImGuiOverlay.h"
#include "core/graphics/vulkan/VulkanFrameInfo.h"

#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace Eternal {
    VulkanImGuiOverlay::VulkanImGuiOverlay(const Builder& builder) {
        m_VulkanPlatform = static_cast<VulkanPlatform*>(builder->platform);
        m_SwapChain = static_cast<VulkanSwapChain*>(builder->swapChain);
        m_Window = builder->window;

        ETERNAL_ASSERT(m_VulkanPlatform != nullptr, "VulkanImGuiLayer :: VulkanPlatform is null");
        ETERNAL_ASSERT(m_SwapChain != nullptr, "VulkanImGuiLayer :: SwapChain is null");
        ETERNAL_ASSERT(m_Window != nullptr, "VulkanImGuiLayer :: Window is null");

        init();
    }

    void VulkanImGuiOverlay::init() {
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

    VulkanImGuiOverlay::~VulkanImGuiOverlay() {
        vk::Device device = m_VulkanPlatform->getLogicalDevice();
        device.waitIdle();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void VulkanImGuiOverlay::beginFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanImGuiOverlay::render(FrameInfo* frameInfo) {
        ImGui::Render();
        auto commandBuffer = static_cast<VulkanFrameInfo*>(frameInfo)->commandBuffer;
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }
}
