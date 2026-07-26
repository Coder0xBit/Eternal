#pragma once
#include "utils/Base.h"
#include "core/window/Window.h"
#include "core/graphics/SwapChain.h"
#include "core/graphics/Backend.h"

namespace Vortak {
    class GraphicsPlatform {
    public:
        virtual ~GraphicsPlatform() = default;

        virtual void initialize() = 0;

        virtual void shutDown() = 0;

        virtual SwapChain* createSwapChain(Window* window) = 0;

        struct BuilderDetails {
            std::string applicationName;
            Backend backend = Backend::Vulkan;
        };

        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class GraphicsPlatform;
            friend class VulkanPlatform;
            friend class OpenGLPlatform;

        public:
            Builder() noexcept;

            Builder(Builder const& rhs) noexcept;

            Builder(Builder&& rhs) noexcept;

            ~Builder() noexcept;

            Builder& operator=(Builder const& rhs) noexcept;

            Builder& operator=(Builder&& rhs) noexcept;

            Builder& applicationName(const std::string& applicationName) noexcept;

            Builder& backend(Backend backend) noexcept;

            std::unique_ptr<GraphicsPlatform> build() const noexcept;
        };

        static std::unique_ptr<GraphicsPlatform> create(const Builder& builder);
    };
}
