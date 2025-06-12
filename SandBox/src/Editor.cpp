#include "Editor.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include <core/graphics/vulkan/VulkanImGuiLayer.h>
#include <core/graphics/vulkan/VulkanGraphicsContext.h>
#include <core/graphics/vulkan/VulkanPlatform.h>
#include <core/graphics/vulkan/VulkanRenderer.h>
#include <core/scene/Entity.h>
#include <core/scene/RenderComponent.h>
#include <core/scene/TransformComponent.h>
#include <core/resource/ResourceManager.h>
#include <core/resource/Mesh.h>


void SetEngineRootDirectory() {
	std::filesystem::path path = std::filesystem::current_path() / "../Eternal";
	std::filesystem::current_path(path);
}

namespace Eternal {

	Editor::Editor()
	{
		Eternal::Logger::Init();

		m_Window = Eternal::Window::Builder()
			.title(std::string("Eternal Application"))
			.height(800)
			.width(1200)
			.build();

		m_Engine = Eternal::Engine::Builder()
			.applicationName("Eternal Application")
			.build();

		m_Scene = Memory::Allocate<Eternal::Scene>();

		addEntity("watch_tower_1", "res/models/wooden_watch_tower.obj", glm::vec3(0.0f, 0.0f, -4.0f));

		//addEntity("cube", "res/models/cube.obj", glm::vec3(0.0f, 0.0f, -10.0f));

		m_Renderer = m_Engine->createRenderer(m_Window, m_Scene);

		m_IsRunning = true;
	}

	void Editor::addEntity(std::string name, std::string filePath, glm::vec3 initialPosition)
	{
		Mesh* mesh = ResourceManager::get().loadResource<Mesh>(filePath);
		if (!mesh) {
			Eternal::Logger::Error("Failed to load mesh from path: {}", filePath);
			return;
		}

		Eternal::Logger::Info("Obj Vertices: {}", mesh->getVertices().size());

		Eternal::Entity model = m_Scene->createEntity();
		model.addComponent<Eternal::IdComponent>();
		model.addComponent<Eternal::NameComponent>(name);
		model.addComponent<Eternal::RenderComponent>(mesh->getVertices(), mesh->getIndices());
		model.addComponent<Eternal::TransformComponent>(initialPosition);
	}

	void Editor::addTriangle()
	{
		Eternal::Entity entity = m_Scene->createEntity();

		std::vector<Eternal::Vertex> vertices = {
			{ 0.0f  , 0.5f  , 0.0f },
			{ -0.5f , -0.5f , 0.0f },
			{ 0.5f  , -0.5f , 0.0f }
		};

		std::vector<uint32_t> indices = { 1, 0, 2 };

		entity.addComponent<Eternal::RenderComponent>(vertices, indices);
	}

	void Editor::addCube()
	{
		std::vector<Vertex> cubeVertices = {
			{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) }, // red
			{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.5f, 0.0f) }, // orange
			{ glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 0.0f) }, // yellow
			{ glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) }, // green	

			{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 1.0f) }, // cyan
			{ glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f) }, // blue
			{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 1.0f) }, // magenta
			{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f) }  // purple
		};

		std::vector<uint32_t> cubeIndices = {
			0, 1, 2, 2, 3, 0, // Front
			3, 2, 5, 5, 4, 3, // Right
			4, 5, 6, 6, 7, 4, // Back
			7, 6, 1, 1, 0, 7, // Side
			1, 6, 5, 5, 2, 1, // Top
			7, 0, 3, 3, 4, 7, // Bottom
		};

		Eternal::Entity cube = m_Scene->createEntity();
		cube.addComponent<Eternal::RenderComponent>(cubeVertices, cubeIndices);
		cube.addComponent<Eternal::TransformComponent>(glm::vec3(0.0f, 0.0f, -10.0f));
	}

	Editor::~Editor()
	{
		shutdown();
	}

	void Editor::run()
	{
		auto vkRenderer = static_cast<VulkanRenderer*>(m_Renderer);

		m_ImGuiLayer = Memory::Allocate<VulkanImGuiLayer>(vkRenderer, m_Window);

		bool showDemoWindow = true;

		while (m_IsRunning)
		{
			m_Window->onUpdate();

			if (FrameInfo* frameInfo = m_Renderer->beginFrame()) {

				m_ImGuiLayer->beginFrame();

				onImGuiRender();

				m_Renderer->render();

				m_ImGuiLayer->render(frameInfo);

				m_Renderer->endFrame();

				Memory::Deallocate(frameInfo);
			}

			m_IsRunning = !m_Window->shouldClose();
		}
	}

	Editor* Editor::create()
	{
		return Memory::Allocate<Editor>();
	}

	void Editor::onImGuiRender()
	{
		ImGui::Begin("Debug Info");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		for (auto e : m_Scene->getAllEntityWith<Eternal::TransformComponent>())
		{
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);

			auto& nameComponent = entity.getComponent<Eternal::NameComponent>();
			ImGui::Text("Entity Name: %s", nameComponent.getName());

			auto& component = entity.getComponent<Eternal::TransformComponent>();
			glm::vec3 translation = component.getTranslation();
			std::string posLabel = "Position " + std::string(nameComponent.getName());
			if (ImGui::DragFloat3(posLabel.c_str(), &translation.x, 0.01f))
			{
				component.setTranslation(translation);
			}

			glm::vec3 rotationDegrees = glm::degrees(component.getRotation());
			std::string rotationLabel = "Rotation " + std::string(nameComponent.getName());
			if (ImGui::SliderFloat3(rotationLabel.c_str(), &rotationDegrees.x, 0.0f, 180.0f, "%.1f"))
			{
				rotationDegrees = glm::clamp(rotationDegrees, 0.0f, 180.0f);
				component.setRotation(glm::radians(rotationDegrees));
			}
		}
		ImGui::End();
	}

	void Editor::shutdown()
	{
		Memory::Deallocate(m_ImGuiLayer);

		Memory::Deallocate(m_Engine);

		Memory::Deallocate(m_Window);
	}
}
