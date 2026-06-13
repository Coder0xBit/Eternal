#pragma once

#include "core/graphics/Timer.h"
#include "core/Camera.h"

namespace Vortak {
    class FrameInfo {
    public:
        virtual ~FrameInfo() = default;

        Vortak::Timestep* timeStep = nullptr;
    };
}
