#include "Viewer.h"

#include "utils/Base.h"

#include "core/scene/Entity.h"
#include "core/scene/TransformComponent.h"
#include "core/scene/MeshComponent.h"
#include "core/event/EventDispatcher.h"
#include "core/graphics/Vertex.h"

#include <imgui/imgui.h>

void SetEngineRootDirectory() {
    std::filesystem::path path = std::filesystem::current_path() / "../Eternal";
    std::filesystem::current_path(path);
}

namespace Eternal {
    Viewer::Viewer() {
        Eternal::Logger::Init();

        mWindow = Eternal::Window::Builder()
                .title(std::string("Eternal Application"))
                .height(800)
                .width(1200)
                .build();

        mInputDispatcher = std::make_unique<InputDispatcher>(mWindow.get());

        mGraphicsPlatform = Eternal::GraphicsPlatform::Builder()
                .applicationName("Eternal Application")
                .backend(mBackend)
                .build();

        mScene = std::make_unique<Eternal::Scene>();
        setupScene();

        mRenderer = Eternal::Renderer::Builder()
                .backend(mBackend)
                .platform(mGraphicsPlatform.get())
                .window(mWindow.get())
                .scene(mScene.get())
                .build();

        mImGuiOverlay = Eternal::ImGuiOverlay::Builder()
                .backend(mBackend)
                .platform(mGraphicsPlatform.get())
                .window(mWindow.get())
                .renderer(mRenderer.get())
                .build();

        mTimer = std::make_unique<Eternal::Timer>();

        mCamera = std::make_unique<Camera>(mInputDispatcher.get());
        float aspectRatio = mWindow->getAspectRatio();
        mCamera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

        mIsRunning = true;
    }

    bool Viewer::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<Eternal::KeyPressedEvent>(ETERNAL_BIND_EVENT_FN(Viewer::onKeyPressed));
        if (mInputCapture.captured) {
            mCamera->onEvent(event);
        }
        return true;
    }

    bool Viewer::onKeyPressed(KeyPressedEvent& event) {
        switch (event.getKeyCode()) {
            case Key::Escape:
                mInputCapture.release();
                return true;
            case Key::P:
                mInputCapture.capture();
                return true;
            default:
                return false;
        }
    }

    void Viewer::handleInputCaptureState() {
        if (!mInputCapture.changed()) {
            return;
        }

        if (mInputCapture.captured) {
            mWindow->setCursorInputMode(CursorInputMode::LOCKED);
        } else {
            mWindow->setCursorInputMode(CursorInputMode::NORMAL);
        }

        mCamera->resetMouseTracking();
        mInputCapture.acknowledge();
    }

    void Viewer::onImGuiRender(Eternal::Timestep& ts) const {
        ImGui::Begin("Debug Info");
        ImGui::Text("Time: %.6f", ts);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        for (auto e: mScene->getAllEntityWith<Eternal::TransformComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, mScene.get());

            auto& nameComponent = entity.getComponent<Eternal::NameComponent>();
            ImGui::Text("Entity Name: %s", nameComponent.getName());

            auto& component = entity.getComponent<Eternal::TransformComponent>();
            glm::vec3 translation = component.getTranslation();
            std::string posLabel = "Position " + std::string(nameComponent.getName());
            if (ImGui::DragFloat3(posLabel.c_str(), &translation.x, 0.01f)) {
                component.setTranslation(translation);
            }

            glm::vec3 rotationDegrees = glm::degrees(component.getRotation());
            std::string rotationLabel = "Rotation " + std::string(nameComponent.getName());
            if (ImGui::SliderFloat3(rotationLabel.c_str(), &rotationDegrees.x, 0.0f, 180.0f, "%.1f")) {
                rotationDegrees = glm::clamp(rotationDegrees, 0.0f, 180.0f);
                component.setRotation(glm::radians(rotationDegrees));
            }
        }

        ImGui::Text("Press Esc to exit Capture Mode. Press P to enter Play/Capture Mode.");
        ImGui::End();
    }

    void Viewer::setupScene() const {
        TestEntityDetails testEntity1 = {
            "watch_tower_1",
            "res/models/wooden_watch_tower.obj",
            "res/models/textures/Wood_Tower_Col.jpg",
            glm::vec3(0.0f, 0.0f, 0.0f)
        };

        mScene->addEntity(testEntity1);

        TestEntityDetails testEntity2 = {
            "watch_tower_2",
            "res/models/wooden_watch_tower.obj",
            "res/models/textures/Wood_Tower_Col.jpg",
            glm::vec3(0.0f, 0.0f, 10.0f)
        };

        mScene->addEntity(testEntity2);

        std::vector<Eternal::Vertex> triVertices = {
            {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 0: top (red)
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 1: bottom-left (green)
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 2: bottom-right (blue)
        };

        std::vector<uint32_t> triIndices = {0, 1, 2};

        Eternal::Entity model = mScene->createEntity("triangle");
        model.addComponent<Eternal::MeshComponent>(triVertices, triIndices);
        model.addComponent<Eternal::TransformComponent>(glm::vec3(0.0f, 10.0f, 0.0f));
    }

    void Viewer::run() {
        mWindow->setEventCallBack(ETERNAL_BIND_EVENT_FN(Viewer::onEvent));
        mTimer->start();
        while (mIsRunning) {
            auto timeStep = mTimer->tick();
            mWindow->onUpdate();
            handleInputCaptureState();
            mCamera->onUpdate(timeStep);

            if (FrameInfo* frameInfo = mRenderer->beginFrame()) {
                mImGuiOverlay->beginFrame();
                onImGuiRender(timeStep);
                mRenderer->render(mCamera.get());
                mImGuiOverlay->render(frameInfo);
                mRenderer->endFrame();
                Memory::Deallocate(frameInfo);
            }

            mIsRunning = !mWindow->shouldClose();
        }
    }

    std::unique_ptr<Viewer> Viewer::create() {
        return std::make_unique<Viewer>();
    }

    Viewer::~Viewer() {
        Viewer::shutdown();
    }

    void Viewer::shutdown() {
    }
}
