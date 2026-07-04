#pragma once

#include "utils/Base.h"
#include "core/graphics/Vertex.h"

namespace Vortak {
    using MeshHandle = uint32_t;

    struct MeshComponent {
    public:
        MeshComponent() = default;

        MeshComponent(std::vector<Vortak::Vertex> vertices, std::vector<uint32_t> indices)
            : mVertices(vertices), mIndices(indices) {
        }

        ~MeshComponent() = default;

        void setMeshHandle(MeshHandle meshHandle) { mMeshHandle = meshHandle; }
        MeshHandle getMeshHandle() const { return mMeshHandle; }

        const std::vector<Vortak::Vertex>& getVertices() const { return mVertices; }
        const std::vector<uint32_t>& getIndices() const { return mIndices; }

    private:
        std::vector<uint32_t> mIndices;
        MeshHandle mMeshHandle = -1; std::vector<Vortak::Vertex> mVertices;

    };
}
