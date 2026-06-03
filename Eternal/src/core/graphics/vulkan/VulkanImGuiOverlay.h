#pragma once

#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/vulkan/VulkanRenderer.h"
#include "core/imgui/ImGuiOverlay.h"
#include "core/window/Window.h"

namespace Eternal {
    class VulkanImGuiOverlay : public ImGuiOverlay {
    public:
        VulkanImGuiOverlay(const Builder& builder);
        ~VulkanImGuiOverlay() override;

        void beginFrame() override;
        void render(FrameInfo* frameInfo) override;

    private:
        VulkanPlatform* mVulkanPlatform = nullptr;
        VulkanRenderer* mVulkanRenderer = nullptr;
        Window* mWindow = nullptr;

        void init();
    };
}
