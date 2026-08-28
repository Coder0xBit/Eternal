#pragma once

#include "utils/Base.h"
#include "core/scene/Scene.h"
#include "core/graphics/VertexBuffer.h"
#include "core/graphics/IndexBuffer.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    struct MeshKey {
        ResourceHandle<Model> modelHandle;
        uint32_t meshIndex = 0;

        bool operator==(const MeshKey& other) const = default;
    };

    struct MeshKeyHasher {
        std::size_t operator()(const Vortak::MeshKey& key) const noexcept {
            std::size_t seed = 0;
            Vortak::hashCombine(seed, key.modelHandle);
            Vortak::hashCombine(seed, key.meshIndex);
            return seed;
        }
    };


    struct MeshBuffer {
        std::unique_ptr<VertexBuffer> vertexBuffer;
        std::unique_ptr<IndexBuffer> indexBuffer;
    };

    class BufferManager {
    public:
        using MeshBuffers = std::unordered_map<MeshKey, MeshBuffer, MeshKeyHasher>;

        BufferManager(GraphicsDevice* graphicsDevice, Backend backend);

        uint32_t getMeshBuffersCount() const {
            return static_cast<uint32_t>(mMeshBuffers.size());
        }

        const MeshBuffer* getMesh(MeshKey meshKey);

        ~BufferManager();

    private :
        MeshBuffer uploadMesh(const Mesh& mesh) const;

    private:
        GraphicsDevice* mGraphicsDevice = nullptr;
        Backend mBackend = Backend::Vulkan;

        MeshBuffers mMeshBuffers;
    };
}
