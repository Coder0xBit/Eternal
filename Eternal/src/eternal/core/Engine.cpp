#include <eternal/core/Engine.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/graphics/Vertex.h>
#include <set>

namespace Eternal {
	Engine::Engine(const Builder& builder)
	{
		m_ApplicationName = builder->applicationName;

		m_Window = Eternal::Window::Builder()
			.title(m_ApplicationName)
			.height(800)
			.width(1200)
			.build();

		std::vector<Eternal::Vertex> vertices = {
			{ 0.0f  , 0.5f  , 0.0f },
			{ -0.5f , -0.5f , 0.0f },
			{ 0.5f  , -0.5f , 0.0f }
		};

		std::vector<uint32_t> indices = { 1, 0, 2 };

		m_GraphicsPlatform = Eternal::GraphicsPlatform::Builder()
			.applicationName(m_ApplicationName)
			.window(m_Window)
			.vertices(vertices)
			.indices(indices)
			.vertexShader("src/eternal/core/graphics/shader/bin/vert.spv")
			.fragmentShader("src/eternal/core/graphics/shader/bin/frag.spv")
			.build();

		m_IsRunning = true;
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