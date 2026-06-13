#pragma  once
#include "utils/Base.h"

namespace Vortak {
    struct RenderTargetDesc {
        uint32_t width = 0;
        uint32_t height = 0;
        bool hasColor = false;
        bool hasDepth = false;
        bool hasStencil = false;
    };

    class RenderTarget {
    public :
        RenderTarget();
        virtual ~RenderTarget() = default;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
    };
}
