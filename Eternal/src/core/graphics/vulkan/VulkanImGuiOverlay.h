#pragma once
#include <core/ImGuiOverlay.h>
#include <core/graphics/vulkan/VulkanPlatform.h>
#include <core/graphics/vulkan/VulkanSwapChain.h>
#include <core/graphics/vulkan/VulkanFrameInfo.h>
#include <core/graphics/vulkan/VulkanRenderer.h>
#include <core/Window.h>

namespace Eternal {
    class VulkanImGuiOverlay : public ImGuiOverlay {
    public:
        VulkanImGuiOverlay(const Builder& builder);
        ~VulkanImGuiOverlay() override;

        void beginFrame() override;
        void render(FrameInfo* frameInfo) override;

    private:
        VulkanPlatform* m_VulkanPlatform = nullptr;
        VulkanSwapChain* m_SwapChain = nullptr;
        Window* m_Window = nullptr;

        void init();
    };
}
