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

		m_IsRunning = true;
	}

	Engine::~Engine()
	{

	}

	Memory::Ref<Renderer> Engine::createRenderer(Memory::Ref<Window> window)
	{
		auto vulkanPlatform = Memory::DynamicPtrCast<VulkanPlatform>(m_GraphicsPlatform);
		if (m_Backend == Backend::Vulkan)
		{
			if (vulkanPlatform)
			{
				m_Renderer = Memory::CreateRef<VulkanRenderer>(vulkanPlatform, window, m_EntityManager);
				return m_Renderer;
			}

			Eternal::Logger::Error("Currently Only supported Backend is Vulkan");
			return nullptr;
		}
		else
		{
			Eternal::Logger::Error("Currently Only supported Backend is Vulkan");
			return nullptr;
		}
	}

	Memory::Ref<EntityManager> Engine::createEntityManager()
	{
		m_EntityManager = Memory::Ref<EntityManager>();
		return m_EntityManager;
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

	Memory::Ref<Engine> Engine::Builder::build() {
		return Memory::CreateRef<Engine>(*this);
	}
}