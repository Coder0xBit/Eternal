#include "core/graphics/opengl/OpenGLVertexBuffer.h"

#include "GLEW/glew.h"

namespace Eternal {
    OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Eternal::Vertex>& vertices) {
        mVertices = vertices;
        glGenBuffers(1, &mVertexBufferID);
        OpenGLVertexBuffer::bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices.data(), GL_STATIC_DRAW);
        OpenGLVertexBuffer::unBind();
    }

    void OpenGLVertexBuffer::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    }

    void OpenGLVertexBuffer::unBind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    uint32_t OpenGLVertexBuffer::getSize() {
        return mVertices.size();
    }
}
