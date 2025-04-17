#pragma once
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR

#include <eternal/utils/Base.h>
#include <eternal/core/Logger.h>
#include <eternal/core/Window.h>
#include <eternal/core/graphics/GraphicsPlatform.h>
#include <eternal/core/ecs/EntityManager.h>
#include <eternal/core/graphics/Renderer.h>
#include <eternal/core/graphics/Backend.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace Eternal {

	class Engine
	{

	public:

		struct BuilderDetails {
			std::string applicationName;
			Backend backend = Backend::Vulkan;
		};

		class Builder : public utils::PrivateImplementation<BuilderDetails> {
			friend class Engine;

		public:
			Builder() noexcept;

			Builder(Builder const& rhs) noexcept;

			Builder(Builder&& rhs) noexcept;

			~Builder() noexcept;

			Builder& operator=(Builder const& rhs) noexcept;

			Builder& operator=(Builder&& rhs) noexcept;

			Builder& applicationName(const std::string& applicationName) noexcept;

			Builder& backend(Backend backend) noexcept;

			Memory::Ref<Engine> build();
		};

		Engine(const Builder& builder);

		~Engine();

		bool isRunning() const { return m_IsRunning; }

		Memory::Ref<Renderer> createRenderer(Memory::Ref<Window> window);

		Memory::Ref<EntityManager> createEntityManager();

		Eternal::GraphicsPlatform* getPlatform() { return m_GraphicsPlatform.get(); }

	private:

		Memory::Ref<Renderer> m_Renderer;

		std::string m_ApplicationName;

		Backend m_Backend = Backend::Vulkan;

		Eternal::Window* m_Window = nullptr;

		Memory::Ref<GraphicsPlatform> m_GraphicsPlatform = nullptr;

		Memory::Ref<EntityManager > m_EntityManager = nullptr;

		bool m_IsRunning = false;
	};
}