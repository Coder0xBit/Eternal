#pragma once

#include "core/resource/Resource.h"
#include "core/graphics/Vertex.h"
#include "core/graphics/VertexBufferLayout.h"

namespace Vortak {
    struct Mesh {
        std::vector<Vortak::Vertex> vertices;
        std::vector<uint32_t> indices;
    };
}
