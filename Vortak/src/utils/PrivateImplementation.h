#pragma once

namespace utils {
    template<typename T>
    class PrivateImplementation {
    public:
        
        template<typename ... Args>
        explicit PrivateImplementation(Args&& ...) noexcept;

        PrivateImplementation() noexcept;

        ~PrivateImplementation() noexcept;

        PrivateImplementation(PrivateImplementation const& rhs) noexcept;

        PrivateImplementation& operator = (PrivateImplementation const& rhs) noexcept;

        PrivateImplementation(PrivateImplementation&& rhs) noexcept : mImpl(rhs.mImpl) { rhs.mImpl = nullptr; }

        PrivateImplementation& operator = (PrivateImplementation&& rhs) noexcept {
            auto temp = mImpl;
            mImpl = rhs.mImpl;
            rhs.mImpl = temp;
            return *this;
        }

    protected:
        T* mImpl = nullptr;
        inline T* operator->() noexcept { return mImpl; }
        inline T const* operator->() const noexcept { return mImpl; }
    };
}
