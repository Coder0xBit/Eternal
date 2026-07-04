#pragma once

#include "utils/Base.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    enum class CommandState {
        InQueue,
        Executing,
        Finished
    };

    class Command {
    public :
        Command() = default;
        ~Command() = default;

        void bindMesh(MeshComponent* meshComponent);

        void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                         int32_t vertexOffset, uint32_t firstInstance);

        CommandState getCommandState() const { return mState; }

    protected:
        CommandState mState = CommandState::InQueue;
        MeshComponent* mMeshComponent = nullptr;
    };
}
