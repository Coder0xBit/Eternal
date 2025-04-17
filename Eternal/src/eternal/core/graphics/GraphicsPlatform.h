#pragma once
#include <eternal/utils/Base.h>	
#include <eternal/core/graphics/Vertex.h>
#include <eternal/core/Window.h>
#include <eternal/core/graphics/SwapChain.h>
#include <eternal/core/ecs/EntityManager.h>
#include <eternal/core/graphics/Backend.h>

namespace Eternal {
	class GraphicsPlatform
	{
	public:
		virtual ~GraphicsPlatform() = default;

		virtual void initialize() = 0;

		virtual void shutDown() = 0;

		virtual Memory::Ref<SwapChain> createSwapChain(Memory::Ref<Window> window) = 0;

		struct BuilderDetails {
			std::vector<Eternal::Vertex> vertices;
			std::string applicationName;
			Backend backend = Backend::Vulkan;
		};

		class Builder : public utils::PrivateImplementation<BuilderDetails>
		{
			friend class GraphicsPlatform;
			friend class VulkanPlatform;

		public:
			Builder() noexcept;

			Builder(Builder const& rhs) noexcept;

			Builder(Builder&& rhs) noexcept;

			~Builder() noexcept;

			Builder& operator=(Builder const& rhs) noexcept;

			Builder& operator=(Builder&& rhs) noexcept;

			Builder& applicationName(const std::string& applicationName) noexcept;

			Builder& backend(Backend backend) noexcept;


			Memory::Ref<GraphicsPlatform> build() noexcept;
		};

		static Memory::Ref<GraphicsPlatform> create(const Builder& builder);
	};
}