#pragma once

#include "core/event/Event.h"

namespace Eternal {
    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : mWidth(width), mHeight(height) {
        }

        unsigned int getWidth() const { return mWidth; }
        unsigned int getHeight() const { return mHeight; }

        EVENT_CLASS_TYPE(WindowResize)

    private:
        unsigned int mWidth, mHeight;
    };
}
