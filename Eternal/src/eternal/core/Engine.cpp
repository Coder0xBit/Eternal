#include <eternal/core/Engine.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <set>

namespace Eternal {
	Engine::Engine(const Builder& builder)
	{
		m_ApplicationName = builder->applicationName;

		m_Window = Eternal::Window::Builder()
			.title(m_ApplicationName)
			.height(600)
			.width(800)
			.build();

		m_GraphicsPlatform = new VulkanPlatform(m_ApplicationName, m_Window);

		m_IsRunning = true;
	}

	void Engine::run()
	{
		while (m_IsRunning)
		{
			m_Window->onUpdate();
			m_GraphicsPlatform->render();
			m_IsRunning = !m_Window->shouldClose();
		}
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