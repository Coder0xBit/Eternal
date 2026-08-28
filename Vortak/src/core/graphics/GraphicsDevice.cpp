#include "core/graphics/GraphicsDevice.h"
#include "core/graphics/vulkan/VulkanDevice.h"
#include "core/graphics/opengl/OpenGLDevice.h"

namespace Vortak {
    std::unique_ptr<GraphicsDevice> GraphicsDevice::create(const Builder& builder) {
        switch (builder->backend) {
            case Backend::Vulkan:
                return std::make_unique<VulkanDevice>(builder);
                break;
            case Backend::OpenGL:
                return std::make_unique<OpenGLDevice>();
                break;
            default:
                return nullptr;
                break;
        }
    }

    GraphicsDevice::Builder::Builder() noexcept = default;

    GraphicsDevice::Builder::Builder(Builder const& rhs) noexcept = default;

    GraphicsDevice::Builder::Builder(Builder&& rhs) noexcept = default;

    GraphicsDevice::Builder::~Builder() noexcept = default;

    GraphicsDevice::Builder& GraphicsDevice::Builder::operator=(Builder const& rhs) noexcept = default;

    GraphicsDevice::Builder& GraphicsDevice::Builder::operator=(Builder&& rhs) noexcept = default;

    GraphicsDevice::Builder& GraphicsDevice::Builder::applicationName(const std::string& applicationName) noexcept {
        mImpl->applicationName = applicationName;
        return *this;
    }

    GraphicsDevice::Builder& GraphicsDevice::Builder::backend(Backend backend) noexcept {
        mImpl->backend = backend;
        return *this;
    }

    std::unique_ptr<GraphicsDevice> GraphicsDevice::Builder::build() const noexcept {
        return create(*this);
    }
}
