#include "GraphicsPlatform.h"


namespace Eternal{

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
	
}

