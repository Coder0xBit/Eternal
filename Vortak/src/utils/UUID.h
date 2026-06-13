#pragma once

#include "utils/Base.h"

namespace Vortak {
    class UUID {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return mUUID; }

    private:
        uint64_t mUUID;
    };
}

namespace std {
    template<typename T>
    struct hash;

    template<>
    struct hash<Vortak::UUID> {
        std::size_t operator()(const Vortak::UUID& uuid) const noexcept {
            return uuid;
        }
    };
}
