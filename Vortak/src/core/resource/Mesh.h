#pragma once

#include "core/resource/Resource.h"
#include "core/graphics/Vertex.h"

namespace Vortak {
    class Mesh : public Resource {
    public:
        Mesh();
        Mesh(std::vector<Vortak::Vertex> vertices, std::vector<uint32_t> indices);
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh&) = delete;
        bool load(const std::string& path) override;
        const std::vector<Vortak::Vertex>& getVertices() const { return mVertices; }
        const std::vector<uint32_t>& getIndices() const { return mIndices; }
        ~Mesh() override;

    private:
        std::vector<Vortak::Vertex> mVertices;
        std::vector<uint32_t> mIndices;
    };
}
