#pragma once

#include "utils/Base.h"
#include "core/scene/Scene.h"
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/IndexBuffer.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    struct MeshBuffer {
        std::unique_ptr<VertexBuffer> vertexBuffer;
        std::unique_ptr<IndexBuffer> indexBuffer;
    };

    class BufferManager {
    public:
        using MeshBuffers = std::unordered_map<ResourceHandle<Mesh>, MeshBuffer>;

        BufferManager(GraphicsPlatform* graphicsPlatform, Backend backend);

        uint32_t getMeshBuffersCount() const {
            return static_cast<uint32_t>(mMeshBuffers.size());
        }

        const MeshBuffer* getMesh(ResourceHandle<Mesh> handle);

        ~BufferManager();

    private :
        MeshBuffer uploadMesh(const Mesh& mesh) const;

    private:
        GraphicsPlatform* mGraphicsPlatform = nullptr;
        Backend mBackend = Backend::Vulkan;

        MeshBuffers mMeshBuffers;
    };
}
