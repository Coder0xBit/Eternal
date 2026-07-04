#pragma once
#include "utils/Base.h"
#include "utils/UUID.h"

namespace Vortak {
    class Resource {
    public:
        Resource() = default;
        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;
        const UUID& getId() const { return mId; }
        void setId(const UUID id) { mId = id; }
        void setPath(const std::string& path) { mPath = path; }
        const std::string& getPath() const { return mPath; }
        virtual bool load(const std::string& path) = 0;
        virtual ~Resource() = default;

    protected:
        UUID mId;
        std::string mPath;
    };
}
