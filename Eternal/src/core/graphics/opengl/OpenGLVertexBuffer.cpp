#include "core/graphics/opengl/OpenGLVertexBuffer.h"
#include "core/graphics/opengl/OpenGLUtils.h"

namespace Eternal {
    OpenGLVertexBuffer::OpenGLVertexBuffer(
        VertexBufferLayout* bufferLayout
    ) : VertexBuffer(bufferLayout) {
        glGenBuffers(1, &mVertexBufferID);
    }

    void OpenGLVertexBuffer::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    }

    void OpenGLVertexBuffer::unBind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::setBuffer(const std::vector<Eternal::Vertex>& vertices) {
        bind();
        mSize = vertices.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        unBind();
    }

    uint32_t OpenGLVertexBuffer::getSize() {
        return mSize;
    }
}
