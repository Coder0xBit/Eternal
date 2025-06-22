#include "GraphicsPlatform.h"
#include <core/graphics/vulkan/VulkanPlatform.h>

namespace Eternal {

	GraphicsPlatform* GraphicsPlatform::create(const Builder& builder) {
		return Memory::Allocate<VulkanPlatform>(builder);
	}

	GraphicsPlatform::Builder::Builder() noexcept = default;
	GraphicsPlatform::Builder::Builder(Builder const& rhs) noexcept = default;
	GraphicsPlatform::Builder::Builder(Builder&& rhs) noexcept = default;
	GraphicsPlatform::Builder::~Builder() noexcept = default;
	GraphicsPlatform::Builder& GraphicsPlatform::Builder::operator=(Builder const& rhs) noexcept = default;
	GraphicsPlatform::Builder& GraphicsPlatform::Builder::operator=(Builder&& rhs) noexcept = default;

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::applicationName(const std::string& applicationName) noexcept {
		mImpl->applicationName = applicationName;
		return *this;
	}

	GraphicsPlatform::Builder& GraphicsPlatform::Builder::backend(Backend backend) noexcept {
		mImpl->backend = backend;
		return *this;
	}

	GraphicsPlatform* GraphicsPlatform::Builder::build() noexcept {
		return GraphicsPlatform::create(*this);
	}
}

