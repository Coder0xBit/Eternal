#include "GraphicsPlatform.h"
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>

namespace Eternal {

	GraphicsPlatform* GraphicsPlatform::create(const Builder& builder) 
	{
		return new VulkanPlatform(builder);
	}

	GraphicsPlatform::Builder::Builder() noexcept = default;

	GraphicsPlatform::Builder::Builder(Builder const& rhs) noexcept = default;

	GraphicsPlatform::Builder::Builder(Builder&& rhs) noexcept = default;

	GraphicsPlatform::Builder::~Builder() noexcept = default;

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::operator=(Builder const& rhs) noexcept = default;

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::operator=(Builder&& rhs) noexcept = default;

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::vertices(const std::vector<Eternal::Vertex>& vertices) noexcept {
		mImpl->vertices = vertices;
		return *this;
	}

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::indices(const std::vector<uint32_t>& indices) noexcept {
		mImpl->indices = indices;
		return *this;
	}

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::vertexShader(const std::string& vertexShaderPath) noexcept {
		mImpl->vertexShaderPath = vertexShaderPath;
		return *this;
	}

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::fragmentShader(const std::string& fragmentShaderPath) noexcept {
		mImpl->fragmentShaderPath = fragmentShaderPath;
		return *this;
	}

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::applicationName(const std::string& applicationName) noexcept {
		mImpl->applicationName = applicationName;
		return *this;
	}

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::window(Eternal::Window* window) noexcept {
		mImpl->window = window;
		return *this;
	}

	GraphicsPlatform* GraphicsPlatform::Builder::build() noexcept {
		return GraphicsPlatform::create(*this);
	}
}

