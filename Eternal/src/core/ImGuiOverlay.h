#pragma once
#include <core/graphics/FrameInfo.h>

#include "Window.h"
#include "graphics/Backend.h"
#include "graphics/GraphicsPlatform.h"

namespace Eternal {
    class ImGuiOverlay {
    public:
        virtual void beginFrame() = 0;

        virtual void render(FrameInfo* frameInfo) = 0;

        virtual ~ImGuiOverlay() = default;

        struct BuilderDetails {
            Backend backend = Vulkan;
            GraphicsPlatform* platform = nullptr;
            Window* window = nullptr;
            SwapChain* swapChain = nullptr;
        };


        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class ImGuiOverlay;
            friend class VulkanImGuiOverlay;

        public:
            Builder() noexcept;

            Builder(Builder const& rhs) noexcept;

            Builder(Builder&& rhs) noexcept;

            ~Builder() noexcept;

            Builder& operator=(Builder const& rhs) noexcept;

            Builder& operator=(Builder&& rhs) noexcept;

            Builder& backend(Backend backend) noexcept;

            Builder& platform(GraphicsPlatform* platform) noexcept;

            Builder& window(Window* window) noexcept;

            Builder& swapChain(SwapChain* swapChain) noexcept;

            ImGuiOverlay* build() const noexcept;
        };

        static ImGuiOverlay* create(const Builder& builder);
    };
}
