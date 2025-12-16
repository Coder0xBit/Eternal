#pragma once

#include "Application.h"

#include "core/imgui/ImGuiOverlay.h"
#include "core/window/Window.h"
#include "core/scene/Scene.h"
#include "core/event/KeyEvents.h"
#include "core/Camera.hpp"
#include "core/graphics/Renderer.h"
#include "core/graphics/Timer.h"

namespace Eternal {
    class Viewer : public Application {
    public:
        Viewer();
        ~Viewer() override;

        static std::unique_ptr<Viewer> create();

        void run() override;
        void shutdown() override;

    private:
        void onImGuiRender(Eternal::Timestep& ts) const;
        void setupScene() const;
        bool onEvent(Event& event);
        bool onKeyPressed(KeyPressedEvent& event);
        void handleInputCaptureState();

    private :
        struct InputCaptureState {
            bool captured = false;
            bool lastApplied = false;
            void capture() { captured = true; }
            void release() { captured = false; }
            bool changed() const { return captured != lastApplied; }
            void acknowledge() { lastApplied = captured; }
        };

        std::unique_ptr<Eternal::GraphicsPlatform> m_GraphicsPlatform;
        std::unique_ptr<Eternal::Renderer> m_Renderer;
        std::unique_ptr<Eternal::Window> m_Window;
        std::unique_ptr<Eternal::InputDispatcher> m_InputDispatcher;
        std::unique_ptr<Eternal::Scene> m_Scene;
        std::unique_ptr<Eternal::SwapChain> m_SwapChain;
        std::unique_ptr<Eternal::ImGuiOverlay> m_ImGuiOverlay;
        std::unique_ptr<Eternal::Camera> m_Camera;
        std::unique_ptr<Eternal::Timer> m_Timer;

        InputCaptureState m_InputCapture;
        Backend m_Backend = Backend::Vulkan;
    };
}
