#pragma once

#include "utils/UUID.h"

namespace Eternal {
    struct IdComponent {
    public:
        IdComponent() : mId(UUID()) {
        }

        IdComponent(const UUID& id) : mId(id) {
        }

        IdComponent(const IdComponent&) = default;
        UUID getId() { return mId; }

    private:
        UUID mId;
    };
}
