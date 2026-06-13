#pragma once
#include <utility>

#include "core/graphics/GraphicsPlatform.h"
#include "core/graphics/Vertex.h"
#include "core/graphics/Backend.h"
#include "core/graphics/VertexBufferLayout.h"
#include "utils/Base.h"

namespace Vortak {
    class VertexBuffer {
    public :
        VertexBuffer(VertexBufferLayout bufferLayout) : bufferLayout(std::move(bufferLayout)) {
        }

        virtual ~VertexBuffer() = default;
        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual uint32_t getSize() = 0;
        virtual void setBuffer(const std::vector<Vortak::Vertex>& vertices) = 0;

        VertexBufferLayout bufferLayout;

        struct BuilderDetails {
            VertexBufferLayout bufferLayout;
            GraphicsPlatform* graphicsPlatform = nullptr;
            Backend backend = Backend::Vulkan;
        };

        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class VertexBuffer;

        public:
            Builder() noexcept;

            Builder(Builder const& rhs) noexcept;

            Builder(Builder&& rhs) noexcept;

            ~Builder() noexcept;

            Builder& operator=(Builder const& rhs) noexcept;

            Builder& operator=(Builder&& rhs) noexcept;

            Builder& graphicsPlatform(GraphicsPlatform* graphicsPlatform) noexcept;

            Builder& backend(Backend backend) noexcept;

            Builder& attribute(Attribute attribute) noexcept;

            std::unique_ptr<VertexBuffer> build() const noexcept;
        };

        static std::unique_ptr<VertexBuffer> create(const Builder& builder);
    };
}
