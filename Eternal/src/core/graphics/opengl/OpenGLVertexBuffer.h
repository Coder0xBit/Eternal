#pragma once
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/Vertex.h"

namespace Eternal {
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(const std::vector<Eternal::Vertex>& vertices);
        void bind() override;
        void unBind() override;
        uint32_t getSize() override;

    private :
        uint32_t mVertexBufferID = 0;
        std::vector<Eternal::Vertex> mVertices;

    };
}
