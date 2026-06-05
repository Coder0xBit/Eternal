#pragma once
#include "VertexBuffer.h"

#include "core/graphics/vulkan/VulkanVertexBuffer.h"
#include "core/graphics/opengl/OpenGLVertexBuffer.h"

namespace Eternal {
    std::unique_ptr<VertexBuffer> VertexBuffer::create(const Builder& builder) {
        switch (builder->backend) {
            case Backend::Vulkan:
                return std::make_unique<VulkanVertexBuffer>(builder->graphicsPlatform, builder->bufferLayout);
                break;
            case Backend::OpenGL:
                return std::make_unique<OpenGLVertexBuffer>(builder->bufferLayout);
                break;
            default:
                return nullptr;
                break;
        }
    }


    VertexBuffer::Builder::Builder() noexcept = default;

    VertexBuffer::Builder::Builder(Builder const& rhs) noexcept = default;

    VertexBuffer::Builder::Builder(Builder&& rhs) noexcept = default;

    VertexBuffer::Builder::~Builder() noexcept = default;

    VertexBuffer::Builder& VertexBuffer::Builder::operator=(Builder const& rhs) noexcept = default;

    VertexBuffer::Builder& VertexBuffer::Builder::operator=(Builder&& rhs) noexcept = default;

    VertexBuffer::Builder& VertexBuffer::Builder::graphicsPlatform(GraphicsPlatform* graphicsPlatform) noexcept {
        mImpl->graphicsPlatform = graphicsPlatform;
        return *this;
    }

    VertexBuffer::Builder& VertexBuffer::Builder::backend(Backend backend) noexcept {
        mImpl->backend = backend;
        return *this;
    }

    VertexBuffer::Builder& VertexBuffer::Builder::attribute(Attribute attribute) noexcept {
        mImpl->bufferLayout.attribute(attribute);
        return *this;
    }

    std::unique_ptr<VertexBuffer> VertexBuffer::Builder::build() const noexcept {
        return create(*this);
    }
}
