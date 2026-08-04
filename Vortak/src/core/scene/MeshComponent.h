#pragma once

#include "utils/Base.h"
#include "core/resource/Model.h"

namespace Vortak {
    struct SubMesh {
        SubMesh(uint32_t index) : meshIndex(index) {}

        uint32_t meshIndex = 0;
    };

    struct MeshComponent {
        ResourceHandle<Model> modelHandle;
        std::vector<SubMesh> subMeshes;
    };
}
