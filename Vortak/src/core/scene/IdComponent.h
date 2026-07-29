#pragma once

#include "utils/UUID.h"

namespace Vortak {
    struct IdComponent {
        UUID id = UUID::generate();
    };
}
