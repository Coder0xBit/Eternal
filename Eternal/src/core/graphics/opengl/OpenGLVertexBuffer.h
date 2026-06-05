#pragma once
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/Vertex.h"


namespace Eternal {
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(VertexBufferLayout bufferLayout);
        void bind() override;
        void unBind() override;
        void setBuffer(const std::vector<Eternal::Vertex>& vertices) override;
        uint32_t getSize() override;

    private :
        uint32_t mVertexBufferID = 0;
        uint32_t mSize = 0;
    };
}
