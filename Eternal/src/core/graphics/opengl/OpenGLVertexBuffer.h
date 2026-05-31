#pragma once
#include "core/graphics/VertexBuffer.h"

namespace Eternal {
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer() = default;
        void bind() override;
        void unBind() override;
        uint32_t getCount() override;

    private :
        uint32_t mVertexBufferID = 0;
    };
}
