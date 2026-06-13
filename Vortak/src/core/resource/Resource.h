#pragma once
#include "utils/Base.h"

namespace Vortak {
    class Resource {
    public:
        Resource() = default;
        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;
        const std::string& getName() const { return mName; }
        const std::string& getPath() const { return mPath; }
        void setPath(const std::string& path) { mPath = path; }
        virtual bool load(const std::string& path) = 0;
        virtual ~Resource() = default;

    protected:
        std::string mName;
        std::string mPath;
    };
}
