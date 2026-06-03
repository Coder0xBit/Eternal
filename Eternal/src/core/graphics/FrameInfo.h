#pragma once

#include "core/graphics/Timer.h"
#include "core/Camera.h"

namespace Eternal {
    class FrameInfo {
    public:
        virtual ~FrameInfo() = default;

        Eternal::Timestep* timeStep = nullptr;
    };
}
