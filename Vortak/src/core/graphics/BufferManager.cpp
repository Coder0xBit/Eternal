#include "core/graphics/BufferManager.h"

#include "core/scene/Entity.h"

namespace Vortak {
    BufferManager::BufferManager(GraphicsPlatform* graphicsPlatform, Backend backend)
        : mGraphicsPlatform(graphicsPlatform), mBackend(backend) {
    }

    BufferManager::~BufferManager() {
        mMeshBuffers.clear();
    }

    MeshHandle BufferManager::addBuffer(const MeshComponent& meshComponent) {
        MeshBuffer meshBuffer;
        meshBuffer.vertexBuffer = VertexBuffer::Builder()
                .graphicsPlatform(mGraphicsPlatform)
                .backend(mBackend)
                .attribute({0, VertexAttribute::POSITON, ElementType::FLOAT3, sizeof(float) * 3, false})
                .attribute({1, VertexAttribute::COLOR, ElementType::FLOAT3, sizeof(float) * 3, false})
                .attribute({2, VertexAttribute::NORMAL, ElementType::FLOAT3, sizeof(float) * 3, false})
                .attribute({3, VertexAttribute::UV, ElementType::FLOAT3, sizeof(float) * 2, false})
                .build();

        const std::vector<Vortak::Vertex>& vertices = meshComponent.getVertices();
        meshBuffer.vertexBuffer->setBuffer(vertices);

        std::vector<uint32_t> indices = meshComponent.getIndices();
        meshBuffer.indexBuffer = IndexBuffer::Builder()
                .graphicsPlatform(mGraphicsPlatform)
                .backend(mBackend)
                .build();

        meshBuffer.indexBuffer->setBuffer(indices);

        const auto handle = static_cast<MeshHandle>(mMeshBuffers.size());

        mMeshBuffers.push_back(std::move(meshBuffer));

        return handle;
    }
}
