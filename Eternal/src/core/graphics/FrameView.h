#pragma once
#include "../Camera.hpp"
#include "core/scene/Scene.h"

namespace Eternal {
    class FrameView {
    public :
        Eternal::Camera* getCamera() { return m_Camera; }
        Eternal::Scene* getScene() { return m_Scene; }
    private :
        Eternal::Camera* m_Camera = nullptr;
        Eternal::Scene* m_Scene = nullptr;
    };
}
