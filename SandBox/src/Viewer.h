#pragma once
#include <core/Engine.h>
#include <core/ImGuiOverlay.h>
#include <core/Window.h>
#include <core/scene/Scene.h>

#include "Application.h"
#include "core/event/KeyEvents.h"
#include "core/Camera.hpp"
#include "core/graphics/Timer.h"

namespace Eternal {
    class Viewer : public Application {
    public:
        Viewer();

        ~Viewer() override;

        static Viewer* create();

        void run() override;

        void shutdown() override;

    private:
        void onImGuiRender(Eternal::Timestep& ts) const;

        void setupScene() const;

        bool onEvent(Event& event) const;

        Engine* m_Engine = nullptr;
        Eternal::Scene* m_Scene = nullptr;
        Eternal::Renderer* m_Renderer = nullptr;
        Eternal::InputDispatcher* m_InputDispatcher = nullptr;
        Window* m_Window = nullptr;
        SwapChain* m_SwapChain = nullptr;
        ImGuiOverlay* m_ImGuiOverlay = nullptr;
        Eternal::Camera* m_Camera = nullptr;
        Timer* m_Timer = nullptr;

        Backend m_Backend = Backend::Vulkan;
    };
}
