#pragma once

#include "core/event/Event.h"

namespace Eternal {
    class MouseMovedEvent : public Event {
    public :
        MouseMovedEvent(const float x, const float y)
            : mMouseX(x), mMouseY(y) {
        }

        float GetX() const { return mMouseX; }
        float GetY() const { return mMouseY; }

        EVENT_CLASS_TYPE(MouseMoved)

    private:
        float mMouseX = 0;
        float mMouseY = 0;
    };
}
