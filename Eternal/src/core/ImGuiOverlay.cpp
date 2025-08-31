#include "ImGuiOverlay.h"

#include "graphics/vulkan/VulkanImGuiOverlay.h"

namespace Eternal {
    ImGuiOverlay* ImGuiOverlay::create(const Builder& builder) {
        ETERNAL_ASSERT(builder->backend == Backend::Vulkan, "Currently Only supported Backend is Vulkan");
        return Memory::Allocate<VulkanImGuiOverlay>(builder);
    }

    ImGuiOverlay::Builder::Builder() noexcept = default;

    ImGuiOverlay::Builder::Builder(Builder const& rhs) noexcept = default;

    ImGuiOverlay::Builder::Builder(Builder&& rhs) noexcept = default;

    ImGuiOverlay::Builder::~Builder() noexcept = default;

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::operator=(Builder const& rhs) noexcept = default;

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::operator=(Builder&& rhs) noexcept = default;

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::backend(const Backend backend) noexcept {
        mImpl->backend = backend;
        return *this;
    }

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::platform(GraphicsPlatform* platform) noexcept {
        mImpl->platform = platform;
        return *this;
    }

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::window(Window* window) noexcept {
        mImpl->window = window;
        return *this;
    }

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::swapChain(SwapChain* swapChain) noexcept {
        mImpl->swapChain = swapChain;
        return *this;
    }

    ImGuiOverlay* ImGuiOverlay::Builder::build() const noexcept {
        return create(*this);
    }
}
