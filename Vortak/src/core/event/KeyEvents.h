#pragma once

#include "core/event/Event.h"
#include "core/input/KeyCodes.h"

namespace Vortak {
    class KeyEvent : public Event {
    public :
        KeyCode getKeyCode() const { return mKeyCode; }

    protected:
        KeyEvent(const KeyCode keyCode) : mKeyCode(keyCode) {
        }

        KeyCode mKeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public :
        KeyPressedEvent(const KeyCode keyCode, const bool repeat) : KeyEvent(keyCode), mIsRepeat(repeat) {
        }

        bool isRepeat() const { return mIsRepeat; }

        EVENT_CLASS_TYPE(KeyPressed)

    private :
        bool mIsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
    public :
        KeyReleasedEvent(const KeyCode keyCode) : KeyEvent(keyCode) {
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };
}
