#include "Renderer.h"

#include "vulkan/VulkanRenderer.h"

namespace Eternal {
    Renderer* Renderer::create(const Builder& builder) {
        ETERNAL_ASSERT(builder->backend == Backend::Vulkan, "Currently Only supported Backend is Vulkan");
        return Memory::Allocate<VulkanRenderer>(builder);
    }

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

    Renderer* Renderer::Builder::build() const noexcept {
        return create(*this);
    }
}
