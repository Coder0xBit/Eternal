#pragma once

#include "core/window/Window.h"

namespace Eternal {
    class InputDispatcher {
    public:
        InputDispatcher() = default;
        InputDispatcher(Eternal::Window* window);
        ~InputDispatcher() = default;

        bool isKeyPressed(Eternal::KeyCode keycode) const;

    private :
        Eternal::Window* m_Window = nullptr;
    };
}
