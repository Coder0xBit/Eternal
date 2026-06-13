#pragma once

#include "utils/Base.h"
#include "core/scene/Scene.h"
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/IndexBuffer.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/RenderComponent.h"

namespace Vortak {
    struct MeshBuffer {
        std::unique_ptr<VertexBuffer> vertexBuffer;
        std::unique_ptr<IndexBuffer> indexBuffer;
    };

    class BufferManager {
    public:
        using MeshBuffers = std::vector<MeshBuffer>;

        BufferManager(GraphicsPlatform* graphicsPlatform, Backend backend);

        VertexBuffer* getVertexBuffer(MeshHandle meshHandle) {
            auto& vertexBuffer = mMeshBuffers[meshHandle].vertexBuffer;
            return vertexBuffer.get();
        }

        IndexBuffer* getIndexBuffer(MeshHandle meshHandle) {
            auto& indexBuffer = mMeshBuffers[meshHandle].indexBuffer;
            return indexBuffer.get();
        }

        uint32_t getMeshBuffersCount() const {
            return static_cast<uint32_t>(mMeshBuffers.size());
        }

        MeshHandle addBuffer(const MeshComponent& meshComponent);

        ~BufferManager();

    private:
        GraphicsPlatform* mGraphicsPlatform = nullptr;
        Backend mBackend = Backend::Vulkan;

        MeshBuffers mMeshBuffers;
    };
}
