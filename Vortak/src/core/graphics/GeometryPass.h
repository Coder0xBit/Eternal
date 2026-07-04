#pragma once
#include "core/graphics/RenderPass.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    class GeometryPass : public RenderPass {
    public:
        GeometryPass(Vortak::Scene* scene) : RenderPass(scene) {
        }

        void build(Vortak::RenderQueue<Command> queue) override;
    };
}
