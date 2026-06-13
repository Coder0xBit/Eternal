#pragma once

#include "core/graphics/Backend.h"

namespace Vortak {
    struct Context {
        Backend backend = Backend::Vulkan;
    };
}
