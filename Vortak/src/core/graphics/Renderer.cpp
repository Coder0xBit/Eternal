#include "core/graphics/Renderer.h"
#include "core/graphics/vulkan/VulkanRenderer.h"

namespace Vortak {
    Renderer::Builder::Builder() noexcept = default;

    Renderer::Builder::Builder(Builder const& rhs) noexcept = default;

    Renderer::Builder::Builder(Builder&& rhs) noexcept = default;

    Renderer::Builder::~Builder() noexcept = default;

    Renderer::Builder& Renderer::Builder::operator=(Builder const& rhs) noexcept = default;

    Renderer::Builder& Renderer::Builder::operator=(Builder&& rhs) noexcept = default;

    Renderer::Builder& Renderer::Builder::backend(const Backend backend) noexcept {
        mImpl->backend = backend;
        return *this;
    }

    Renderer::Builder& Renderer::Builder::platform(GraphicsPlatform* platform) noexcept {
        mImpl->platform = platform;
        return *this;
    }

    Renderer::Builder& Renderer::Builder::window(Window* window) noexcept {
        mImpl->window = window;
        return *this;
    }

    Renderer::Builder& Renderer::Builder::scene(Scene* scene) noexcept {
        mImpl->scene = scene;
        return *this;
    }

    std::unique_ptr<Renderer> Renderer::Builder::build() const noexcept {
        return std::make_unique<VulkanRenderer>(*this);
    }

}
