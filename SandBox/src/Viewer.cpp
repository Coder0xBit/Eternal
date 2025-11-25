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

        m_InputDispatcher = Memory::Allocate<InputDispatcher>(m_Window);

        m_Engine = Eternal::Engine::Builder()
                .applicationName("Eternal Application")
                .backend(m_Backend)
                .build();

        m_Scene = Memory::Allocate<Eternal::Scene>();
        setupScene();

        m_Renderer = m_Engine->createRenderer(m_Window, m_Scene);

        m_ImGuiOverlay = Eternal::ImGuiOverlay::Builder()
                .backend(m_Backend)
                .platform(m_Engine->getPlatform())
                .window(m_Window)
                .swapChain(m_Renderer->getSwapChain())
                .build();

        m_Timer = Memory::Allocate<Eternal::Timer>();

        m_Camera = Memory::Allocate<Camera>(m_InputDispatcher);
        float aspectRatio = m_Window->getAspectRatio();
        m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

        m_IsRunning = true;
    }

    bool Viewer::onEvent(Event& event) const {
        m_Camera->onEvent(event);
        return true;
    }

    void Viewer::onImGuiRender(Eternal::Timestep& ts) const {
        ImGui::Begin("Debug Info");
        ImGui::Text("Time: %.6f", ts);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        for (auto e: m_Scene->getAllEntityWith<Eternal::TransformComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene);

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
            m_Camera->onUpdate(timeStep);

            if (m_Renderer->beginFrame()) {
                m_Renderer->render(m_Camera);
                m_Renderer->endFrame();
            }

            m_IsRunning = !m_Window->shouldClose();
        }
    }

    Viewer* Viewer::create() {
        return Memory::Allocate<Viewer>();
    }

    Viewer::~Viewer() {
        Viewer::shutdown();
    }

    void Viewer::shutdown() {
        Memory::Deallocate(m_Timer);
        Memory::Deallocate(m_ImGuiOverlay);
        Memory::Deallocate(m_Engine);
        Memory::Deallocate(m_Window);
        Memory::Deallocate(m_Camera);
    }
}
