#pragma once

#include "core/graphics/Timer.h"
#include "core/Camera.hpp"

namespace Eternal {
    class FrameInfo {
    public:
        virtual ~FrameInfo() = default;

        Eternal::Timestep* timeStep = nullptr;
    };
}
