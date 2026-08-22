#include "core/graphics/Renderer.h"

#include "core/scene/Entity.h"

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

        mBufferManager = std::make_unique<BufferManager>(mGraphicsPlatform, mBackend);
    }

    bool Renderer::beginFrame() { return true; }

    void Renderer::render(Vortak::Camera* camera, Vortak::Scene* scene) {
        auto view = scene->getAllEntityWith<Vortak::MeshComponent>();
        for (auto& handle : view) {
            Entity e = Entity(handle, scene);
            auto mesh = e.getComponent<Vortak::MeshComponent>();

            for (auto& subMeshes : mesh.subMeshes) {
                auto meshKey = MeshKey(mesh.modelHandle, subMeshes.meshIndex);
                auto meshBuffer = mBufferManager->getMesh(meshKey);
            }
        }
    }

    void Renderer::endFrame() {}

    SwapChain* Renderer::getSwapChain() const { return nullptr; }

    Renderer::~Renderer() {}
}
