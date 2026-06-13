#pragma once

#include "core/event/Event.h"

namespace Vortak {
    class EventDispatcher {
    public :
        EventDispatcher(Event& event) : mEvent(event) {
        }

        template<typename T, typename F>
        bool dispatch(const F& func) {
            if (mEvent.getEventType() == T::GetStaticType()) {
                mEvent.handled |= func(static_cast<T&>(mEvent));
                return true;
            }
            return false;
        }

    private :
        Event& mEvent;
    };
}
