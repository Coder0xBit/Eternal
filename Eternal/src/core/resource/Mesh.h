#pragma once

#include <utils/Base.h>
#include <core/resource/Resource.h>
#include <core/graphics/Vertex.h>

namespace Eternal {
    class Mesh : public Resource {
    public:
        Mesh();
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh&) = delete;
        bool load(const std::string& path) override;
        const std::vector<Eternal::Vertex>& getVertices() const { return m_Vertices; }
        const std::vector<uint32_t>& getIndices() const { return m_Indices; }
        virtual ~Mesh();

    private:
        std::vector<Eternal::Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
    };
}
