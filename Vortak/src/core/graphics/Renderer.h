#pragma once

#include "utils/Base.h"
#include "core/graphics/Command.h"
#include "core/graphics/RenderQueue.h"
#include "core/graphics/SwapChain.h"
#include "core/graphics/FrameInfo.h"
#include "core/graphics/Backend.h"
#include "core/graphics/GraphicsPlatform.h"
#include "core/scene/Scene.h"

namespace Vortak {
    class Renderer {
    public:
        virtual FrameInfo* beginFrame() = 0;

        virtual void render(Vortak::Camera* camera) = 0;

        virtual void endFrame() = 0;

        virtual ~Renderer() = default;

        virtual SwapChain* getSwapChain() const = 0;

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

            std::unique_ptr<Renderer> build() const noexcept;
        };

    private :
        Vortak::RenderQueue<Command> mRenderQueue;
    };
}
