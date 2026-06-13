#pragma once

#include "core/window/Window.h"

namespace Vortak {
    class InputDispatcher {
    public:
        InputDispatcher() = default;
        InputDispatcher(Vortak::Window* window);
        ~InputDispatcher() = default;

        bool isKeyPressed(Vortak::KeyCode keycode) const;

    private :
        Vortak::Window* mWindow = nullptr;
    };
}
