#pragma once
#include "utils/UUID.h"

namespace Vortak {
    template <typename T>
    class ResourceHandle {
    public :
        constexpr ResourceHandle() = default;

        explicit constexpr ResourceHandle(UUID id) : mId(id) {}

        constexpr UUID uuid() const { return mId; }

        constexpr bool valid() const { return mId.valid(); }

        explicit constexpr operator bool() const noexcept { return valid(); }

        auto operator<=>(const ResourceHandle&) const = default;

    private :
        UUID mId;
    };
}

namespace std {
    template <typename T>
    struct hash<Vortak::ResourceHandle<T>> {
        size_t operator()(const Vortak::ResourceHandle<T>& handle) const noexcept {
            return std::hash<Vortak::UUID>{}(handle.uuid());
        }
    };
}
