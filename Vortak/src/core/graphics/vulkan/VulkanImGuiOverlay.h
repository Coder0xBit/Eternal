#pragma once

#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/imgui/ImGuiOverlay.h"
#include "core/window/Window.h"

namespace Vortak {
    class VulkanImGuiOverlay : public ImGuiOverlay {
    public:
        VulkanImGuiOverlay(const Builder& builder);

        ~VulkanImGuiOverlay() override;

        void beginFrame() override;

        void render() override;

    private:
        VulkanPlatform* mVulkanPlatform = nullptr;
        Window* mWindow = nullptr;

        void init();
    };
}
