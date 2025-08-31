#include <core/Engine.h>
#include <core/ImGuiOverlay.h>
#include <core/Window.h>
#include <core/scene/Scene.h>

#include "Application.h"
#include "core/event/KeyEvents.h"
#include "core/graphics/Camera.hpp"

namespace Eternal {
    class Editor : public Application {
    public:
        Editor();

        ~Editor() override;

        static Editor* create();

        void run() override;

        void shutdown() override;

    private:
        void onImGuiRender() const;

        void setupScene() const;

        bool onEvent(Event& event) const;

        bool onKeyPressed(Eternal::KeyPressedEvent& event) const;

        Engine* m_Engine = nullptr;
        Eternal::Scene* m_Scene = nullptr;
        Eternal::Renderer* m_Renderer = nullptr;
        Window* m_Window = nullptr;
        SwapChain* m_SwapChain = nullptr;
        ImGuiOverlay* m_ImGuiOverlay = nullptr;

        Backend m_Backend = Backend::Vulkan;
    };
}
