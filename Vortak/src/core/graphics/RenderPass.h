#pragma once
#include "core/graphics/Command.h"
#include "core/graphics/RenderQueue.h"
#include "core/scene/Scene.h"

namespace Vortak {
    class RenderPass {
    public :
        RenderPass(Vortak::Scene* scene) : mScene(scene) {
        }

        RenderPass(const RenderPass& renderPass) = delete;
        RenderPass& operator=(const RenderPass& renderPass) = delete;

        RenderPass(RenderPass&& renderPass) = delete;
        RenderPass& operator=(RenderPass&& renderPass) = delete;

        virtual ~RenderPass();

        virtual void build(Vortak::RenderQueue<Command> queue) = 0;

    protected :
        Vortak::Scene* mScene;
    };
}
