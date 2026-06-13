#pragma once

#include "core/graphics/IndexBuffer.h"
#include "core/graphics/VertexBuffer.h"

namespace Vortak {
    class Mesh {
    public:
        Mesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
            : mVertexBuffer(std::move(vertexBuffer)), mIndexBuffer(std::move(indexBuffer)) {
        }

        const std::shared_ptr<VertexBuffer>& getVertexBuffer() const {
            return mVertexBuffer;
        }

        const std::shared_ptr<IndexBuffer>& getIndexBuffer() const {
            return mIndexBuffer;
        }

        uint32_t getIndexCount() const {
            return mIndexBuffer->getCount();
        }

    private:
        std::shared_ptr<VertexBuffer> mVertexBuffer;
        std::shared_ptr<IndexBuffer> mIndexBuffer;
    };
}
