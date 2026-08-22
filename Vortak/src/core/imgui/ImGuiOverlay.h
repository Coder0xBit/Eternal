#pragma once

#include "core/graphics/Backend.h"
#include "core/graphics/GraphicsPlatform.h"
#include "core/graphics/Renderer.h"
#include "core/window/Window.h"

namespace Vortak {
    class ImGuiOverlay {
    public:
        virtual void beginFrame() = 0;

        virtual void render() = 0;

        virtual ~ImGuiOverlay() = default;

        struct BuilderDetails {
            Backend backend = Vulkan;
            GraphicsPlatform* platform = nullptr;
            Window* window = nullptr;
            Renderer* renderer = nullptr;
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

            Builder& renderer(Renderer* renderer) noexcept;

            std::unique_ptr<ImGuiOverlay> build() const noexcept;
        };

        static std::unique_ptr<ImGuiOverlay> create(const Builder& builder);
    };
}
