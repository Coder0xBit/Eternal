#include "core/input/InputDispatcher.h"

namespace Eternal {
    InputDispatcher::InputDispatcher(Eternal::Window* window) : mWindow(window) {
    }

    bool InputDispatcher::isKeyPressed(Eternal::KeyCode keycode) const {
        return mWindow->isKeyPressed(keycode);
    }
}
