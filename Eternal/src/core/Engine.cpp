#include <core/Engine.h>
#include <core/graphics/vulkan/VulkanPlatform.h>
#include <core/graphics/Vertex.h>
#include <core/graphics/vulkan/VulkanRenderer.h>
#include <core/scene/Scene.h>

#include <set>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Eternal {
    Engine::Engine(const Builder& builder) {
        m_ApplicationName = builder->applicationName;

        m_Backend = builder->backend;
        ETERNAL_ASSERT(m_Backend == Backend::Vulkan, "Currently Only supported Backend is Vulkan");

        m_GraphicsPlatform = Eternal::GraphicsPlatform::Builder()
                .applicationName(m_ApplicationName)
                .backend(m_Backend)
                .build();
    }

    Engine::~Engine() {
        Memory::Deallocate(m_Renderer);
    }

    Renderer* Engine::createRenderer(Window* window, Scene* scene) {
        m_Renderer = Eternal::Renderer::Builder()
                .backend(m_Backend)
                .platform(m_GraphicsPlatform)
                .window(window)
                .scene(scene)
                .build();

        return m_Renderer;
    }

    Engine::Builder::Builder() noexcept = default;

    Engine::Builder::Builder(Builder const& rhs) noexcept = default;

    Engine::Builder::Builder(Builder&& rhs) noexcept = default;

    Engine::Builder::~Builder() noexcept = default;

    Engine::Builder& Engine::Builder::operator=(Builder const& rhs) noexcept = default;

    Engine::Builder& Engine::Builder::operator=(Builder&& rhs) noexcept = default;

    Engine::Builder& Engine::Builder::applicationName(const std::string& applicationName) noexcept {
        mImpl->applicationName = applicationName;
        return *this;
    }

    Engine::Builder& Engine::Builder::backend(Backend backend) noexcept {
        mImpl->backend = backend;
        return *this;
    }

    Engine* Engine::Builder::build() {
        return Memory::Allocate<Engine>(*this);
    }
}
