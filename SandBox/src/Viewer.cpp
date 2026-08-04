#include "Viewer.h"

#include "utils/Base.h"

#include "core/scene/Entity.h"
#include "core/scene/TransformComponent.h"
#include "core/scene/MeshComponent.h"
#include "core/event/EventDispatcher.h"
#include "core/graphics/Vertex.h"

#include <imgui/imgui.h>

#include "assimp/code/AssetLib/3MF/3MFXmlTags.h"
#include "core/resource/Model.h"
#include "core/resource/ResourceManager.h"
#include "core/scene/importer/ModelImporter.h"

void SetEngineRootDirectory() {
    std::filesystem::path path = std::filesystem::current_path() / "../Vortak";
    std::filesystem::current_path(path);
}

namespace Vortak {
    Viewer::Viewer() {
        Vortak::Logger::Init();

        mWindow = Vortak::Window::Builder()
                 .title(std::string("Vortak Application"))
                 .height(800)
                 .width(1200)
                 .backend(mBackend)
                 .build();

        mInputDispatcher = std::make_unique<InputDispatcher>(mWindow.get());

        mGraphicsPlatform = Vortak::GraphicsPlatform::Builder()
                           .applicationName("Vortak Application")
                           .backend(mBackend)
                           .build();

        mScene = std::make_unique<Vortak::Scene>();
        setupScene();

        mRenderer = Vortak::Renderer::Builder()
                   .backend(mBackend)
                   .platform(mGraphicsPlatform.get())
                   .window(mWindow.get())
                   .build();

        // mImGuiOverlay = Vortak::ImGuiOverlay::Builder()
        //                .backend(mBackend)
        //                .platform(mGraphicsPlatform.get())
        //                .window(mWindow.get())
        //                .renderer(mRenderer.get())
        //                .build();

        mTimer = std::make_unique<Vortak::Timer>();

        mCamera = std::make_unique<Camera>(mInputDispatcher.get());
        float aspectRatio = mWindow->getAspectRatio();
        mCamera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

        mIsRunning = true;
    }

    bool Viewer::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<Vortak::KeyPressedEvent>(VORTAK_BIND_EVENT_FN(Viewer::onKeyPressed));
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

    void Viewer::onImGuiRender(Vortak::Timestep& ts) const {
        ImGui::Begin("Debug Info");
        ImGui::Text("Time: %.6f", ts);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        for (auto e : mScene->getAllEntityWith<Vortak::TransformComponent>()) {
            Vortak::Entity entity = Vortak::Entity(e, mScene.get());

            auto& nameComponent = entity.getComponent<Vortak::NameComponent>();
            ImGui::Text("Entity Name: %s", nameComponent.name);

            auto& component = entity.getComponent<Vortak::TransformComponent>();
            glm::vec3 translation = component.translation;
            std::string posLabel = "Position " + std::string(nameComponent.name);
            if (ImGui::DragFloat3(posLabel.c_str(), &translation.x, 0.01f)) {
                component.translation = translation;
            }

            glm::vec3 rotationDegrees = glm::degrees(component.rotation);
            std::string rotationLabel = "Rotation " + std::string(nameComponent.name);
            if (ImGui::SliderFloat3(rotationLabel.c_str(), &rotationDegrees.x, 0.0f, 180.0f, "%.1f")) {
                rotationDegrees = glm::clamp(rotationDegrees, 0.0f, 180.0f);
                component.rotation = glm::radians(rotationDegrees);
            }
        }

        ImGui::Text("Press Esc to exit Capture Mode. Press P to enter Play/Capture Mode.");
        ImGui::End();
    }

    void Viewer::setupScene() const {
        // TestEntityDetails testEntity1 = {
        //     "watch_tower_1",
        //     "res/models/tower/wooden_watch_tower.obj",
        //     "res/models/tower/Wood_Tower_Col.jpg",
        //     glm::vec3(0.0f, 0.0f, 0.0f)
        // };
        //
        // TestEntityDetails testEntity2 = {
        //     "watch_tower_2",
        //     "res/models/tower/wooden_watch_tower.obj",
        //     "res/models/tower/Wood_Tower_Col.jpg",
        //     glm::vec3(0.0f, 0.0f, 10.0f)
        // };
        // std::vector<Vortak::Vertex> triVertices = {
        //     {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 0: top (red)
        //     {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 1: bottom-left (green)
        //     {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 2: bottom-right (blue)
        // };
        //
        // std::vector<uint32_t> triIndices = {0, 1, 2};
        //
        // Vortak::Entity model = mScene->createEntity("triangle");
        // Mesh* mesh = Memory::Allocate<Mesh>(triVertices, triIndices);
        // model.addComponent<Vortak::MeshComponent>(mesh);
        // model.addComponent<Vortak::TransformComponent>(glm::vec3(0.0f, 10.0f, 0.0f));

        std::string filePath = "res/models/sponza/Sponza.gltf";

        auto sponzaModel = ResourceManager::get().load<Model>(filePath);
        ModelImporter::import(sponzaModel.handle , mScene.get());
        // if (!sponzaModel) {
        //     Vortak::Logger::Error("Failed to load model from path: {}", filePath);
        //     return;
        // }
        //
        // mScene->addModel(sponzaModel);
    }

    void Viewer::run() {
        mWindow->setEventCallBack(VORTAK_BIND_EVENT_FN(Viewer::onEvent));
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

    void Viewer::shutdown() {}
}
