#pragma once

#include "utils/Base.h"
#include "core/graphics/Vertex.h"

namespace Eternal {
    struct MeshComponent {
    public:
        MeshComponent() = default;

        MeshComponent(std::vector<Eternal::Vertex> vertices, std::vector<uint32_t> indices)
            : mVertices(vertices), mIndices(indices) {
        }

        ~MeshComponent() = default;

        const std::vector<Eternal::Vertex>& getVertices() const { return mVertices; }
        const std::vector<uint32_t>& getIndices() const { return mIndices; }

    private:
        std::vector<Eternal::Vertex> mVertices;
        std::vector<uint32_t> mIndices;
    };
}
