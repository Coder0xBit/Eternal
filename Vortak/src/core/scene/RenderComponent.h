#pragma once
#include <cstdint>

namespace Vortak {
    using MeshHandle = uint32_t;

    class RenderComponent {
    public :
        RenderComponent() = default;

        RenderComponent(MeshHandle meshHandle) : mMeshHandle(meshHandle) {
        };

        MeshHandle getMeshHandle() const { return mMeshHandle; };

    private :
        MeshHandle mMeshHandle = -1;
    };
}
