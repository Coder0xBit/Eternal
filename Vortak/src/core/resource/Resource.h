#pragma once
#include "utils/Base.h"
#include "utils/UUID.h"
#include "core/resource/ResourceHandle.h"

namespace Vortak {
    template <typename Derived>
    class Resource {
    public:
        using ResourceHandleType = ResourceHandle<Derived>;

        Resource() = default;

        Resource(const Resource&) = delete;

        Resource& operator=(const Resource&) = delete;

        const ResourceHandleType& getHandle() const { return mHandle; }

        void setHandle(const ResourceHandleType handle) { mHandle = handle; }

        void setPath(const std::string& path) { mPath = path; }

        const std::string& getPath() const { return mPath; }

        virtual bool load(const std::string& path) = 0;

        virtual ~Resource() = default;

    protected:
        ResourceHandleType mHandle;
        std::string mPath;
    };
}
