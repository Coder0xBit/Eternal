#pragma once
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/Vertex.h"


namespace Vortak {
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(VertexBufferLayout bufferLayout);

        void bind() override;

        void unBind() override;

        void setBuffer(const std::vector<Vortak::Vertex>& vertices) override;

        uint32_t getSize() override;

    private :
        uint32_t mVertexBufferId = 0;
        uint32_t mVertexArrayId = 0;
        uint32_t mSize = 0;
    };
}
