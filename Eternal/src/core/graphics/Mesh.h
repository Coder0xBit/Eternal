#pragma once

#include "core/graphics/IndexBuffer.h"
#include "core/graphics/VertexBuffer.h"

namespace Eternal {
    class Mesh {
    public:
        Mesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
            : m_VertexBuffer(std::move(vertexBuffer)), m_IndexBuffer(std::move(indexBuffer)) {
        }

        const std::shared_ptr<VertexBuffer>& getVertexBuffer() const {
            return m_VertexBuffer;
        }

        const std::shared_ptr<IndexBuffer>& getIndexBuffer() const {
            return m_IndexBuffer;
        }

        uint32_t getIndexCount() const {
            return m_IndexBuffer->getCount();
        }

    private:
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}
