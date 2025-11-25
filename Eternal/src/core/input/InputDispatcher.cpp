#include "core/input/InputDispatcher.h"

namespace Eternal {
    InputDispatcher::InputDispatcher(Eternal::Window* window) : m_Window(window) {
    }

    bool InputDispatcher::isKeyPressed(Eternal::KeyCode keycode) const {
        return m_Window->isKeyPressed(keycode);
    }
}
