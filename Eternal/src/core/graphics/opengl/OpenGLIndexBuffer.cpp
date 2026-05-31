#include "core/graphics/opengl/OpenGLIndexBuffer.h"

namespace Eternal {
    OpenGLIndexBuffer::OpenGLIndexBuffer(const std::vector<uint32_t>& indices) {
        glGenBuffers(1, &mIndexBufferID);
        mIndexCount = indices.size();
        OpenGLIndexBuffer::bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount, indices.data(), GL_STATIC_DRAW);
        OpenGLIndexBuffer::unBind();
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        glDeleteBuffers(1, &mIndexBufferID);
        mIndexBufferID = 0;
    }

    void OpenGLIndexBuffer::bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
    }

    void OpenGLIndexBuffer::unBind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
