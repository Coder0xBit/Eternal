#pragma once
#include <core/graphics/SwapChain.h>
#include <core/graphics/FrameInfo.h>

#include <utils/Base.h>

#include "Backend.h"
#include "GraphicsPlatform.h"
#include "core/scene/Scene.h"

namespace Eternal {
    class Renderer {
    public:
        virtual FrameInfo* beginFrame() = 0;

        virtual void render() = 0;

        virtual void endFrame() = 0;

        virtual ~Renderer() = default;

        struct BuilderDetails {
            Backend backend = Vulkan;
            GraphicsPlatform* platform = nullptr;
            Window* window = nullptr;
            Scene* scene = nullptr;
        };

        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class Renderer;
            friend class VulkanRenderer;

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

            Builder& scene(Scene* scene) noexcept;

            Renderer* build() const noexcept;
        };

        static Renderer* create(Builder const& builder);
    };
}
