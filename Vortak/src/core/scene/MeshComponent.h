#pragma once

#include "utils/Base.h"
#include "core/graphics/Vertex.h"
#include "core/resource/Mesh.h"

namespace Vortak {
    struct MeshComponent {
    public:
        MeshComponent(ResourceHandle<Mesh> resourceHandle) : mMeshHandle(resourceHandle) {};

        ~MeshComponent() = default;

        ResourceHandle<Mesh> getHandle() const { return mMeshHandle; }

    private:
        ResourceHandle<Mesh> mMeshHandle;
    };
}
