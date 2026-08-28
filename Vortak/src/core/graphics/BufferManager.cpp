#include "core/graphics/BufferManager.h"

#include "core/resource/ResourceManager.h"
#include "core/scene/Entity.h"

namespace Vortak {
    BufferManager::BufferManager(GraphicsDevice* graphicsDevice, Backend backend)
        : mGraphicsDevice(graphicsDevice), mBackend(backend) {}

    const MeshBuffer* BufferManager::getMesh(MeshKey meshKey) {
        ResourceHandle<Model> handle = meshKey.modelHandle;
        auto it = mMeshBuffers.find(meshKey);

        if (it != mMeshBuffers.end()) { return &it->second; }

        auto* model = ResourceManager::get().get<Model>(handle);
        auto* mesh = model->meshes[meshKey.meshIndex].get();

        if (!model || !mesh) { return nullptr; }

        auto [iter, inserted] =
            mMeshBuffers.emplace(meshKey, uploadMesh(*mesh));

        return &iter->second;
    }

    BufferManager::~BufferManager() {
        mMeshBuffers.clear();
    }

    MeshBuffer BufferManager::uploadMesh(const Mesh& mesh) const {
        MeshBuffer meshBuffer;
        meshBuffer.vertexBuffer = VertexBuffer::Builder()
                                 .graphicsDevice(mGraphicsDevice)
                                 .backend(mBackend)
                                  // .layout(mesh.getVertexBufferLayout())
                                 .build();

        const std::vector<Vortak::Vertex>& vertices = mesh.vertices;
        meshBuffer.vertexBuffer->setBuffer(vertices);

        std::vector<uint32_t> indices = mesh.indices;
        meshBuffer.indexBuffer = IndexBuffer::Builder()
                                .graphicsDevice(mGraphicsDevice)
                                .backend(mBackend)
                                .build();

        meshBuffer.indexBuffer->setBuffer(indices);
        return meshBuffer;
    }
}
