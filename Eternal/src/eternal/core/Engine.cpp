#include <eternal/core/Engine.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/graphics/Vertex.h>
#include<eternal/core/graphics/vulkan/VulkanRenderer.h>
#include <set>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


namespace Eternal {
	Engine::Engine(const Builder& builder)
	{
		m_ApplicationName = builder->applicationName;

		m_Backend = builder->backend;

		m_GraphicsPlatform = Eternal::GraphicsPlatform::Builder()
			.applicationName(m_ApplicationName)
			.backend(m_Backend)
			.build();
	}

	Engine::~Engine()
	{
		delete m_Renderer;
	}

	Renderer* Engine::createRenderer(Window* window, EntityManager* entityManager)
	{
		if (m_Backend == Backend::Vulkan)
		{
			auto vulkanPlatform = dynamic_cast<VulkanPlatform*>(m_GraphicsPlatform);
			if (vulkanPlatform)
			{
				m_Renderer = Memory::Allocate<VulkanRenderer>(vulkanPlatform, window, entityManager);
				return m_Renderer;
			}

			Eternal::Logger::Error("Casting issue in createRenderer(...)");
			return nullptr;
		}
		else
		{
			Eternal::Logger::Error("Currently Only supported Backend is Vulkan");
			return nullptr;
		}
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

	Engine::Builder& Engine::Builder::backend(Backend backend) noexcept {
		mImpl->backend = backend;
		return *this;
	}

	Engine* Engine::Builder::build() {
		return Memory::Allocate<Engine>(*this);
	}
}