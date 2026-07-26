#include "core/graphics/GraphicsPlatform.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/opengl/OpenGLPlatform.h"

namespace Vortak {
    std::unique_ptr<GraphicsPlatform> GraphicsPlatform::create(const Builder& builder) {
        switch (builder->backend) {
            case Backend::Vulkan:
                return std::make_unique<VulkanPlatform>(builder);
                break;
            case Backend::OpenGL:
                return std::make_unique<OpenGLPlatform>();
                break;
            default:
                return nullptr;
                break;
        }
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

    std::unique_ptr<GraphicsPlatform> GraphicsPlatform::Builder::build() const noexcept {
        return create(*this);
    }
}
