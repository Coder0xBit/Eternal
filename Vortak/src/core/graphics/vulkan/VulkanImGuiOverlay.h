#pragma once

#include "core/graphics/vulkan/VulkanDevice.h"
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
        VulkanDevice* mVulkanPlatform = nullptr;
        Window* mWindow = nullptr;

        void init();
    };
}
