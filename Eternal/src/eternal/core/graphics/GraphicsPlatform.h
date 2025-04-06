#pragma once
#include <eternal/utils/Base.h>	
#include <eternal/core/graphics/Vertex.h>
#include <eternal/core/Window.h>
#include <eternal/core/graphics/Vertex.h>

namespace Eternal {
	class GraphicsPlatform
	{
	public:
		virtual ~GraphicsPlatform() = default;

		virtual void initialize() = 0;

		virtual void shutDown() = 0;

		virtual void render() = 0;

		struct BuilderDetails {
			std::vector<Eternal::Vertex> vertices;
			std::vector<uint32_t> indices;
			std::string vertexShaderPath;
			std::string fragmentShaderPath;
			std::string applicationName;
			Eternal::Window* window;
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

			Builder& vertices(const std::vector<Eternal::Vertex>& vertices) noexcept;

			Builder& indices(const std::vector<uint32_t>& indices) noexcept;

			Builder& vertexShader(const std::string& vertexShaderPath) noexcept;

			Builder& fragmentShader(const std::string& fragmentShaderPath) noexcept;

			Builder& applicationName(const std::string& applicationName) noexcept;

			Builder& window(Eternal::Window* window) noexcept;

			GraphicsPlatform* build() noexcept;
		};

		static GraphicsPlatform* create(const Builder& builder);
	};
}