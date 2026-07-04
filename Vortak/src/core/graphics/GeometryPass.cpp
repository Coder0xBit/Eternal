#include "core/graphics/GeometryPass.h"
#include "core/graphics/Command.h"

namespace Vortak {
    void GeometryPass::build(Vortak::RenderQueue<Command> queue) {
        auto view = mScene->getAllEntityWith<Vortak::MeshComponent>();

        for (auto& entity: view) {
            Vortak::Command command;

        }
    }
}
