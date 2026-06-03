#pragma once

#include "core/graphics/Backend.h"

namespace Eternal {
    struct Context {
        Backend backend = Backend::Vulkan;
    };
}
