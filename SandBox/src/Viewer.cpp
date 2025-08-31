#include "Viewer.h"
#include "imgui/imgui.h"

#include <core/graphics/vulkan/VulkanImGuiOverlay.h>
#include <core/graphics/vulkan/VulkanPlatform.h>
#include <core/graphics/vulkan/VulkanRenderer.h>
#include <core/scene/Entity.h>
#include <core/scene/TransformComponent.h>

#include "core/event/EventDispatcher.h"
#include "core/event/KeyEvents.h"


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

        m_IsRunning = true;
    }

    bool Viewer::onEvent(Event& event) const {
        m_Renderer->onEvent(event);
        return true;
    }

    void Viewer::onImGuiRender() const {
        ImGui::Begin("Debug Info");
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
    }

    void Viewer::run() {
        m_Window->setEventCallBack(ETERNAL_BIND_EVENT_FN(Viewer::onEvent));

        while (m_IsRunning) {
            m_Window->onUpdate();

            if (FrameInfo* frameInfo = m_Renderer->beginFrame()) {
                m_ImGuiOverlay->beginFrame();
                onImGuiRender();
                m_Renderer->render();
                m_ImGuiOverlay->render(frameInfo);
                m_Renderer->endFrame();
                Memory::Deallocate(frameInfo);
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
        Memory::Deallocate(m_ImGuiOverlay);
        Memory::Deallocate(m_Engine);
        Memory::Deallocate(m_Window);
    }
}
