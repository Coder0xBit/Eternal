#pragma once

#include "utils/Base.h"
#include "log/Logger.h"
#include "window/Window.h"
#include "core/graphics/GraphicsPlatform.h"
#include "core/graphics/Renderer.h"
#include "core/scene/Scene.h"
#include "core/graphics/Backend.h"

namespace Eternal {
    class Engine {
    public:
        struct BuilderDetails {
            std::string applicationName;
            Backend backend = Backend::Vulkan;
        };

        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class Engine;

        public:
            Builder() noexcept;
            Builder(Builder const& rhs) noexcept;
            Builder(Builder&& rhs) noexcept;
            ~Builder() noexcept;
            Builder& operator=(Builder const& rhs) noexcept;
            Builder& operator=(Builder&& rhs) noexcept;
            Builder& applicationName(const std::string& applicationName) noexcept;
            Builder& backend(Backend backend) noexcept;
            Engine* build();
        };

        Engine(const Builder& builder);
        ~Engine();
        Renderer* createRenderer(Window* window, Scene* scene);
        Renderer* getRenderer() { return m_Renderer; }
        Eternal::GraphicsPlatform* getPlatform() const { return m_GraphicsPlatform; }

    private:
        Renderer* m_Renderer;
        std::string m_ApplicationName;
        Backend m_Backend = Backend::Vulkan;
        GraphicsPlatform* m_GraphicsPlatform = nullptr;
    };
}
