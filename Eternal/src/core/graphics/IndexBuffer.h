#pragma once
#include "utils/Base.h"

namespace Eternal {
    class IndexBuffer {
    public :
        virtual ~IndexBuffer() = default;
        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual uint32_t getCount() = 0;
    };
}
