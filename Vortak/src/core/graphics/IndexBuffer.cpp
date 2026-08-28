#include "core/graphics/IndexBuffer.h"
#include "core/graphics/opengl/OpenGLIndexBuffer.h"
#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Vortak {
    std::unique_ptr<IndexBuffer> IndexBuffer::create(const Builder& builder) {
        switch (builder->backend) {
            case Backend::Vulkan:
                return std::make_unique<VulkanIndexBuffer>(builder->graphicsDevice);
                break;
            case Backend::OpenGL:
                return std::make_unique<OpenGLIndexBuffer>();
                break;
            default:
                return nullptr;
                break;
        }
    }


    IndexBuffer::Builder::Builder() noexcept = default;

    IndexBuffer::Builder::Builder(Builder const& rhs) noexcept = default;

    IndexBuffer::Builder::Builder(Builder&& rhs) noexcept = default;

    IndexBuffer::Builder::~Builder() noexcept = default;

    IndexBuffer::Builder& IndexBuffer::Builder::operator=(Builder const& rhs) noexcept = default;

    IndexBuffer::Builder& IndexBuffer::Builder::operator=(Builder&& rhs) noexcept = default;

    IndexBuffer::Builder& IndexBuffer::Builder::graphicsDevice(GraphicsDevice* graphicsDevice) noexcept {
        mImpl->graphicsDevice = graphicsDevice;
        return *this;
    }

    IndexBuffer::Builder& IndexBuffer::Builder::backend(Backend backend) noexcept {
        mImpl->backend = backend;
        return *this;
    }

    std::unique_ptr<IndexBuffer> IndexBuffer::Builder::build() const noexcept {
        return create(*this);
    }
}
