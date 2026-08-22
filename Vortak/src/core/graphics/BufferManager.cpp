#include "core/graphics/BufferManager.h"

#include "core/resource/ResourceManager.h"
#include "core/scene/Entity.h"

namespace Vortak {
    BufferManager::BufferManager(GraphicsPlatform* graphicsPlatform, Backend backend)
        : mGraphicsPlatform(graphicsPlatform), mBackend(backend) {}

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
                                 .graphicsPlatform(mGraphicsPlatform)
                                 .backend(mBackend)
                                  // .layout(mesh.getVertexBufferLayout())
                                 .build();

        const std::vector<Vortak::Vertex>& vertices = mesh.vertices;
        meshBuffer.vertexBuffer->setBuffer(vertices);

        std::vector<uint32_t> indices = mesh.indices;
        meshBuffer.indexBuffer = IndexBuffer::Builder()
                                .graphicsPlatform(mGraphicsPlatform)
                                .backend(mBackend)
                                .build();

        meshBuffer.indexBuffer->setBuffer(indices);
        return meshBuffer;
    }
}
