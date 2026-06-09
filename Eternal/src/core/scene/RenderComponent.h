#pragma once
#include <cstdint>

namespace Eternal {
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
