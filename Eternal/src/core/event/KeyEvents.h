#pragma once
#include "Event.h"
#include "core/input/KeyCodes.h"

namespace Eternal {
    class KeyEvent : public Event {
    public :
        KeyCode getKeyCode() const { return m_KeyCode; }

    protected:
        KeyEvent(const KeyCode keyCode) : m_KeyCode(keyCode) {
        }

        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public :
        KeyPressedEvent(const KeyCode keyCode, const bool repeat) : KeyEvent(keyCode), m_IsRepeat(repeat) {
        }

        bool isRepeat() const { return m_IsRepeat; }

        EVENT_CLASS_TYPE(KeyPressed)

    private :
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
    public :
        KeyReleasedEvent(const KeyCode keyCode) : KeyEvent(keyCode) {
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };
}
