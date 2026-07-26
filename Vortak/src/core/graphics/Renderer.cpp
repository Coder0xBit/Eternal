#include "core/graphics/Renderer.h"

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

    std::unique_ptr<Renderer> Renderer::Builder::build() const noexcept {
        return std::make_unique<Renderer>(*this);
    }

    Renderer::Renderer(const Builder& builder) noexcept {
        mGraphicsPlatform = builder.mImpl->platform;
        mWindow = builder.mImpl->window;
        mBackend = builder.mImpl->backend;
    }

    FrameInfo* Renderer::beginFrame() { return nullptr; }

    void Renderer::render(Vortak::Camera* camera) {}

    void Renderer::endFrame() {}

    SwapChain* Renderer::getSwapChain() const { return nullptr; }

    Renderer::~Renderer() {}
}
