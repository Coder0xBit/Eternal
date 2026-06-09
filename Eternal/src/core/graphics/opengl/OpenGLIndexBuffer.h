#pragma once

#include "core/graphics/IndexBuffer.h"

namespace Eternal {
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer();
        void bind() override;
        void unBind() override;
        uint32_t getCount() override {return mIndexCount;}
        void setBuffer(const std::vector<uint32_t>& indices) override;
        ~OpenGLIndexBuffer() override;

    private :
        uint32_t mIndexCount = 0;
        uint32_t mIndexBufferID = 0;
    };
}
