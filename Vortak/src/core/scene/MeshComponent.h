#pragma once

#include "utils/Base.h"
#include "core/graphics/Vertex.h"
#include "core/resource/Mesh.h"

namespace Vortak {
    struct MeshComponent {
        ResourceHandle<Mesh> meshHandle;
    };
}
