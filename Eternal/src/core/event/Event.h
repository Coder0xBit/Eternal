#pragma once
#include <utils/Base.h>

namespace Eternal {
    enum EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };


#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                                virtual EventType getEventType() const override { return EventType::type; }\
                                virtual const char* getName() const override { return #type; }

    class Event {
    public :
        bool handled = false;

        virtual ~Event() = default;

        virtual EventType getEventType() const = 0;

        virtual const char* getName() const = 0;

        virtual std::string toString() { return getName(); }
    };
}
