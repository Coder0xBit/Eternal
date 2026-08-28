#include "core/imgui/ImGuiOverlay.h"
#include "core/graphics/vulkan/VulkanImGuiOverlay.h"

namespace Vortak {
    std::unique_ptr<ImGuiOverlay> ImGuiOverlay::create(const Builder& builder) {
        VORTAK_ASSERT(builder->backend == Backend::Vulkan, "Currently Only supported Backend is Vulkan");
        return std::make_unique<VulkanImGuiOverlay>(builder);
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

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::platform(GraphicsDevice* platform) noexcept {
        mImpl->platform = platform;
        return *this;
    }

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::window(Window* window) noexcept {
        mImpl->window = window;
        return *this;
    }

    ImGuiOverlay::Builder& ImGuiOverlay::Builder::renderer(Renderer* renderer) noexcept {
        mImpl->renderer = renderer;
        return *this;
    }

    std::unique_ptr<ImGuiOverlay> ImGuiOverlay::Builder::build() const noexcept {
        return create(*this);
    }
}
