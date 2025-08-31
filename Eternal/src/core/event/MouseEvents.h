#pragma once
#include "Event.h"
#include "core/input/KeyCodes.h"

namespace Eternal {
    class MouseMovedEvent : public Event {
    public :
        MouseMovedEvent(const float x, const float y)
            : m_MouseX(x), m_MouseY(y) {
        }

        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        EVENT_CLASS_TYPE(MouseMoved)

    private:
        float m_MouseX = 0;
        float m_MouseY = 0;
    };
}
