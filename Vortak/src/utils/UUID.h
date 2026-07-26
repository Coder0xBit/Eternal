#pragma once

#include "utils/Base.h"

namespace Vortak {
    class UUID {
    public:
        /// Creates an invalid UUID (value = 0)
        constexpr UUID() noexcept = default;

        explicit constexpr UUID(uint64_t value) noexcept
            : mUUID(value) {}

        UUID(const UUID&) = default;

        UUID(UUID&&) noexcept = default;

        UUID& operator=(const UUID&) = default;

        UUID& operator=(UUID&&) noexcept = default;

        ~UUID() = default;

        [[nodiscard]] static UUID generate();

        [[nodiscard]] constexpr uint64_t value() const noexcept {
            return mUUID;
        }

        [[nodiscard]]
        constexpr bool valid() const noexcept { return mUUID != 0; }

        explicit constexpr operator bool() const noexcept { return valid(); }

        explicit constexpr operator uint64_t() const noexcept { return mUUID; }

        auto operator<=>(const UUID&) const = default;

    private:
        uint64_t mUUID = 0;
    };
}

namespace std {
    template <typename T>
    struct hash;

    template <>
    struct hash<Vortak::UUID> {
        std::size_t operator()(const Vortak::UUID& uuid) const noexcept {
            return std::hash<uint64_t>{}(uuid.value());
        }
    };
}
