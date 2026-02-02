#pragma once
#include "utils/Base.h"

namespace Eternal {
    class VertexBuffer {
    public :
        virtual ~VertexBuffer() = default;
        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual uint32_t getCount() = 0;
    };
}
