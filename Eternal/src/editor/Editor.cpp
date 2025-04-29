#include "Editor.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include <eternal/core/graphics/vulkan/VulkanImGuiLayer.h>
#include <eternal/core/graphics/vulkan/VulkanGraphicsContext.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/graphics/vulkan/VulkanRenderer.h>
#include <eternal/core/scene/Entity.h>
#include <eternal/core/scene/RenderComponent.h>
#include <eternal/core/scene/TransformComponent.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <eternal/utils/TinyObjLoader.h>

namespace std {
	template <>
	struct hash<Eternal::Vertex> {
		size_t operator()(Eternal::Vertex const& vertex) const {
			size_t seed = 0;
			Eternal::hashCombine(seed, vertex.position.x, vertex.position.y, vertex.position.z, vertex.color.x, vertex.color.y, vertex.color.z);
			return seed;
		}
	};
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

		setupEntities();

		m_IsRunning = true;
	}

	void Editor::setupEntities()
	{
		std::string filepath = "res/models/wooden_watch_tower.obj";

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		std::vector<Eternal::Vertex> vertices;
		std::vector<uint32_t> indices;


		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			ETERNAL_ASSERT(true, warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

		Eternal::Logger::Info("Obj Vertices: {}", vertices.size());

		Eternal::Entity model = m_Scene->createEntity();
		model.addComponent<Eternal::IdComponent>(UUID());
		model.addComponent<Eternal::NameComponent>("wooden_watch_tower");
		model.addComponent<Eternal::RenderComponent>(vertices, indices);
		model.addComponent<Eternal::TransformComponent>(glm::vec3(0.0f, 0.0f, -4.0f));
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
		cube.addComponent<Eternal::TransformComponent>(glm::vec3(0.0f, 0.0f, -4.0f));
	}

	Editor::~Editor()
	{
		shutdown();
	}

	void Editor::run()
	{
		auto renderer = m_Engine->createRenderer(m_Window, m_Scene);

		auto vkRenderer = static_cast<VulkanRenderer*>(renderer);

		auto swapchain = vkRenderer->getSwapChain();

		auto renderPass = static_cast<VulkanSwapChain*>(swapchain)->getRenderPass();

		auto platform = vkRenderer->getPlatform();

		m_ImGuiLayer = Memory::Allocate<VulkanImGuiLayer>(platform, static_cast<VulkanSwapChain*>(swapchain), m_Window);

		while (m_IsRunning)
		{
			m_Window->onUpdate();

			if (Renderer::FrameInfo* frameInfo = renderer->beginFrame()) {

				m_ImGuiLayer->beginFrame();

				onImGuiRender();

				m_ImGuiLayer->render();

				renderer->render(frameInfo);

				renderer->endFrame();
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
			if (ImGui::DragFloat3("Position", &translation.x, 0.01f))
			{
				component.setTranslation(translation);
			}

			glm::vec3 rotationDegrees = glm::degrees(component.getRotation());
			if (ImGui::SliderFloat3("Rotation", &rotationDegrees.x, 0.0f, 180.0f, "%.1f"))
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
