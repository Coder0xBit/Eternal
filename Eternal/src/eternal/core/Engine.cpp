#include <eternal/core/Engine.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/graphics/Vertex.h>
#include <set>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <eternal/utils/TinyObjLoader.h>


namespace std {
	template <>
	struct hash<Eternal::Vertex> {
		size_t operator()(Eternal::Vertex const& vertex) const {
			size_t seed = 0;
			Eternal::hashCombine(seed, vertex.position, vertex.color);
			return seed;
		}
	};
}

namespace Eternal {
	Engine::Engine(const Builder& builder)
	{
		m_ApplicationName = builder->applicationName;

		m_Window = Eternal::Window::Builder()
			.title(m_ApplicationName)
			.height(800)
			.width(1200)
			.build();

		m_EntityManager = new EntityManager();

		setupEntities();

		m_GraphicsPlatform = Eternal::GraphicsPlatform::Builder()
			.applicationName(m_ApplicationName)
			.window(m_Window)
			.entityManager(m_EntityManager)
			.build();

		m_IsRunning = true;
	}

	void Engine::setupEntities()
	{
		std::string filepath = "res/models/flat_vase.obj";

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

		Eternal::Entity model = m_EntityManager->createEntity();
		m_EntityManager->addComponent<Eternal::RenderComponent>(model, vertices, indices);
		m_EntityManager->addComponent<Eternal::TransformComponent>(model, glm::vec3(0.0f, 0.3f, -6.0f));

	}

	void Engine::addTriangle()
	{
		Eternal::Entity e = m_EntityManager->createEntity();

		std::vector<Eternal::Vertex> vertices = {
			{ 0.0f  , 0.5f  , 0.0f },
			{ -0.5f , -0.5f , 0.0f },
			{ 0.5f  , -0.5f , 0.0f }
		};

		std::vector<uint32_t> indices = { 1, 0, 2 };

		m_EntityManager->addComponent<Eternal::RenderComponent>(e, vertices, indices);

	}

	void Engine::addCube()
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

		Eternal::Entity cube = m_EntityManager->createEntity();
		m_EntityManager->addComponent<Eternal::RenderComponent>(cube, cubeVertices, cubeIndices);
		m_EntityManager->addComponent<Eternal::TransformComponent>(cube, glm::vec3(0.0f, 0.0f, -4.0f));
	}

	void Engine::onUpdate()
	{
		m_Window->onUpdate();
		m_GraphicsPlatform->render();
		m_IsRunning = !m_Window->shouldClose();
	}

	vk::CommandBuffer Engine::getCommandBuffer()
	{
		return static_cast<VulkanPlatform*>(m_GraphicsPlatform)->getCommandBuffer();
	}

	Engine::~Engine()
	{
		delete m_Window;
		delete m_GraphicsPlatform;
		delete m_EntityManager;
	}

	Engine::Builder::Builder() noexcept = default;

	Engine::Builder::Builder(Builder const& rhs) noexcept = default;

	Engine::Builder::Builder(Builder&& rhs) noexcept = default;

	Engine::Builder::~Builder() noexcept = default;

	Engine::Builder& Engine::Builder::operator=(Builder const& rhs) noexcept = default;

	Engine::Builder& Engine::Builder::operator=(Builder&& rhs) noexcept = default;

	Engine::Builder& Engine::Builder::applicationName(const std::string& applicationName) noexcept {
		mImpl->applicationName = applicationName;
		return *this;
	}

	Engine* Engine::Builder::build() {
		return new Engine(*this);
	}
}