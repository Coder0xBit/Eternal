#include "core/graphics/vulkan/VulkanImGuiOverlay.h"
#include "core/graphics/vulkan/VulkanFrameInfo.h"

#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_glfw.h>


namespace Vortak {
    VulkanImGuiOverlay::VulkanImGuiOverlay(const Builder& builder) {
        mVulkanPlatform = dynamic_cast<VulkanPlatform*>(builder->platform);
        mWindow = builder->window;

        VORTAK_ASSERT(mVulkanPlatform != nullptr, "VulkanImGuiLayer :: VulkanPlatform is null");
        VORTAK_ASSERT(mWindow != nullptr, "VulkanImGuiLayer :: Window is null");

        init();
    }

    void VulkanImGuiOverlay::init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = mVulkanPlatform->getVkInstance();
        initInfo.PhysicalDevice = mVulkanPlatform->getPhysicalDevice();
        initInfo.Device = mVulkanPlatform->getLogicalDevice();
        initInfo.Queue = mVulkanPlatform->getGraphicsQueue();
        /**
         * Need to look into the handling this using the Render Target so probably this will be automatically managed by the Render Graph
         */
        // initInfo.RenderPass = mVulkanRenderer->getRenderPass();
        initInfo.DescriptorPoolSize = 2;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = 2;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        ImGui_ImplVulkan_Init(&initInfo);

        GLFWwindow* nativeWindow = static_cast<GLFWwindow*>(mWindow->getNativeWindow());
        ImGui_ImplGlfw_InitForVulkan(nativeWindow, true);
        ImGui_ImplVulkan_CreateFontsTexture();
    }

    VulkanImGuiOverlay::~VulkanImGuiOverlay() {
        vk::Device device = mVulkanPlatform->getLogicalDevice();
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
