#pragma once

#include "Application.h"

#include "core/graphics/opengl/OpenGLPlatform.h"
#include "core/graphics/opengl/OpenGLShader.h"

#include "core/imgui/ImGuiOverlay.h"
#include "core/window/Window.h"
#include "core/scene/Scene.h"
#include "core/event/KeyEvents.h"
#include "core/Camera.h"
#include "core/graphics/Renderer.h"
#include "core/graphics/Timer.h"

namespace Vortak {
    class Viewer : public Application {
    public:
        Viewer();
        ~Viewer() override;

        static std::unique_ptr<Viewer> create();

        void run() override;
        void shutdown() override;

    private:
        void onImGuiRender(Vortak::Timestep& ts) const;
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

        std::unique_ptr<Vortak::GraphicsPlatform> mGraphicsPlatform;
        std::unique_ptr<Vortak::Renderer> mRenderer;
        std::unique_ptr<Vortak::Window> mWindow;
        std::unique_ptr<Vortak::InputDispatcher> mInputDispatcher;
        std::unique_ptr<Vortak::Scene> mScene;
        std::unique_ptr<Vortak::SwapChain> mSwapChain;
        std::unique_ptr<Vortak::ImGuiOverlay> mImGuiOverlay;
        std::unique_ptr<Vortak::Camera> mCamera;
        std::unique_ptr<Vortak::Timer> mTimer;

        InputCaptureState mInputCapture;
        Backend mBackend = Backend::Vulkan;
    };
}
