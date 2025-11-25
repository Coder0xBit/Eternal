#pragma once

#include "core/event/Event.h"

namespace Eternal {
    class EventDispatcher {
    public :
        EventDispatcher(Event& event) : m_Event(event) {
        }

        template<typename T, typename F>
        bool dispatch(const F& func) {
            if (m_Event.getEventType() == T::GetStaticType()) {
                m_Event.handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private :
        Event& m_Event;
    };
}
