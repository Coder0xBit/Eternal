#pragma once
#include <GLEW/glew.h>
#include "core/graphics/IndexBuffer.h"

namespace Eternal {
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(const std::vector<uint32_t>& indices);
        void bind() override;
        void unBind() override;
        uint32_t getCount() override;

        ~OpenGLIndexBuffer() override;
    private :
        uint32_t mIndexCount;
        uint32_t mIndexBufferID;
    };
}
