#include "core/graphics/BufferManager.h"

#include "core/resource/ResourceManager.h"
#include "core/scene/Entity.h"

namespace Vortak {
    BufferManager::BufferManager(GraphicsPlatform* graphicsPlatform, Backend backend)
        : mGraphicsPlatform(graphicsPlatform), mBackend(backend) {}

    const MeshBuffer* BufferManager::getMesh(ResourceHandle<Mesh> handle) {
        auto it = mMeshBuffers.find(handle);

        if (it != mMeshBuffers.end()) {
            return &it->second;
        }

        Mesh* mesh = ResourceManager::get().get<Mesh>(handle);

        if (!mesh) {
            return nullptr;
        }

        auto [iter, inserted] =
            mMeshBuffers.emplace(handle, uploadMesh(*mesh));

        return &iter->second;
    }

    BufferManager::~BufferManager() {
        mMeshBuffers.clear();
    }

    MeshBuffer BufferManager::uploadMesh(const Mesh& mesh) const {
        MeshBuffer meshBuffer;
        meshBuffer.vertexBuffer = VertexBuffer::Builder()
                                 .graphicsPlatform(mGraphicsPlatform)
                                 .backend(mBackend)
                                 .layout(mesh.getVertexBufferLayout())
                                 .build();

        const std::vector<Vortak::Vertex>& vertices = mesh.getVertices();
        meshBuffer.vertexBuffer->setBuffer(vertices);

        std::vector<uint32_t> indices = mesh.getIndices();
        meshBuffer.indexBuffer = IndexBuffer::Builder()
                                .graphicsPlatform(mGraphicsPlatform)
                                .backend(mBackend)
                                .build();

        meshBuffer.indexBuffer->setBuffer(indices);
        return meshBuffer;
    }
}
