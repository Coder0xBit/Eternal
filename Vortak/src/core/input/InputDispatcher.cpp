#include "core/input/InputDispatcher.h"

namespace Vortak {
    InputDispatcher::InputDispatcher(Vortak::Window* window) : mWindow(window) {
    }

    bool InputDispatcher::isKeyPressed(Vortak::KeyCode keycode) const {
        return mWindow->isKeyPressed(keycode);
    }
}
