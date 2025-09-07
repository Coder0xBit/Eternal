#pragma once
#include <utils/Base.h>


#include "Timer.h"
#include "core/Camera.hpp"

namespace Eternal {
    class FrameInfo {
    public:
        virtual ~FrameInfo() = default;

        Eternal::Camera* camera;
        Eternal::Timestep* timeStep = nullptr;
    };
}
