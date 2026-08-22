#pragma once

#include "utils/Base.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    class Command {
    public :
        Command() = default;

        ~Command() = default;

        void bindMesh(MeshComponent* meshComponent);

        void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                         int32_t vertexOffset, uint32_t firstInstance);

    protected:
        MeshComponent* mMeshComponent = nullptr;
    };
}
