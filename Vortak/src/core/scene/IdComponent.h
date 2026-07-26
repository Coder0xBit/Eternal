#pragma once

#include "utils/UUID.h"

namespace Vortak {
    struct IdComponent {
    public:
        IdComponent() : mId(UUID::generate()) {}

        IdComponent(const UUID& id) : mId(id) {}

        IdComponent(const IdComponent&) = default;

        UUID getId() { return mId; }

    private:
        UUID mId;
    };
}
