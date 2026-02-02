#include "Viewer.h"

#include "utils/Base.h"

#include "core/scene/Entity.h"
#include "core/scene/TransformComponent.h"
#include "core/scene/RenderComponent.h"
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

        m_Window = Eternal::Window::Builder()
                .title(std::string("Eternal Application"))
                .height(800)
                .width(1200)
                .build();

        m_InputDispatcher = std::make_unique<InputDispatcher>(m_Window.get());

        m_GraphicsPlatform = Eternal::GraphicsPlatform::Builder()
                .applicationName("Eternal Application")
                .backend(m_Backend)
                .build();

        m_Scene = std::make_unique<Eternal::Scene>();
        setupScene();

        m_Renderer = Eternal::Renderer::Builder()
                .backend(m_Backend)
                .platform(m_GraphicsPlatform.get())
                .window(m_Window.get())
                .scene(m_Scene.get())
                .build();

        m_ImGuiOverlay = Eternal::ImGuiOverlay::Builder()
                .backend(m_Backend)
                .platform(m_GraphicsPlatform.get())
                .window(m_Window.get())
                .renderer(m_Renderer.get())
                .build();

        m_Timer = std::make_unique<Eternal::Timer>();

        m_Camera = std::make_unique<Camera>(m_InputDispatcher.get());
        float aspectRatio = m_Window->getAspectRatio();
        m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

        m_IsRunning = true;
    }

    bool Viewer::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<Eternal::KeyPressedEvent>(ETERNAL_BIND_EVENT_FN(Viewer::onKeyPressed));
        if (m_InputCapture.captured) {
            m_Camera->onEvent(event);
        }
        return true;
    }

    bool Viewer::onKeyPressed(KeyPressedEvent& event) {
        switch (event.getKeyCode()) {
            case Key::Escape:
                m_InputCapture.release();
                return true;
            case Key::P:
                m_InputCapture.capture();
                return true;
            default:
                return false;
        }
    }

    void Viewer::handleInputCaptureState() {
        if (!m_InputCapture.changed()) {
            return;
        }

        if (m_InputCapture.captured) {
            m_Window->setCursorInputMode(CursorInputMode::LOCKED);
        } else {
            m_Window->setCursorInputMode(CursorInputMode::NORMAL);
        }

        m_Camera->resetMouseTracking();
        m_InputCapture.acknowledge();
    }

    void Viewer::onImGuiRender(Eternal::Timestep& ts) const {
        ImGui::Begin("Debug Info");
        ImGui::Text("Time: %.6f", ts);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        for (auto e: m_Scene->getAllEntityWith<Eternal::TransformComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene.get());

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

        m_Scene->addEntity(testEntity1);

        TestEntityDetails testEntity2 = {
            "watch_tower_2",
            "res/models/wooden_watch_tower.obj",
            "res/models/textures/Wood_Tower_Col.jpg",
            glm::vec3(0.0f, 0.0f, 10.0f)
        };

        m_Scene->addEntity(testEntity2);

        std::vector<Eternal::Vertex> triVertices = {
            {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 0: top (red)
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 1: bottom-left (green)
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 2: bottom-right (blue)
        };

        std::vector<uint32_t> triIndices = {0, 1, 2};

        Eternal::Entity model = m_Scene->createEntity("triangle");
        model.addComponent<Eternal::RenderComponent>(triVertices, triIndices);
        model.addComponent<Eternal::TransformComponent>(glm::vec3(0.0f, 10.0f, 0.0f));
    }

    void Viewer::run() {
        m_Window->setEventCallBack(ETERNAL_BIND_EVENT_FN(Viewer::onEvent));
        m_Timer->start();
        while (m_IsRunning) {
            auto timeStep = m_Timer->tick();
            m_Window->onUpdate();
            handleInputCaptureState();
            m_Camera->onUpdate(timeStep);

            if (FrameInfo* frameInfo = m_Renderer->beginFrame()) {
                m_ImGuiOverlay->beginFrame();
                onImGuiRender(timeStep);
                m_Renderer->render(m_Camera.get());
                m_ImGuiOverlay->render(frameInfo);
                m_Renderer->endFrame();
                Memory::Deallocate(frameInfo);
            }

            m_IsRunning = !m_Window->shouldClose();
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
