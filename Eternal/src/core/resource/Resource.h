#pragma once
#include "utils/Base.h"

namespace Eternal {
    class Resource {
    public:
        Resource() = default;
        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;
        const std::string& getName() const { return m_Name; }
        const std::string& getPath() const { return m_Path; }
        void setPath(const std::string& path) { m_Path = path; }
        virtual bool load(const std::string& path) = 0;
        virtual ~Resource() = default;

    protected:
        std::string m_Name;
        std::string m_Path;
    };
}
