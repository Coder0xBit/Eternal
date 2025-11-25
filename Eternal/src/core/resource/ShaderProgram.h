#pragma once

#include "utils/Base.h"
#include "core/resource/Resource.h"

namespace Eternal {
    // This is basic Shader file resource class that can be loaded from a file.
    class ShaderProgram : public Resource {
        using Blob = std::vector<char>;

    public:
        ShaderProgram();
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        bool load(const std::string& path) override;
        const uint32_t* getBlob() { return reinterpret_cast<uint32_t*>(m_Blob.data()); }
        size_t getBlobSize() const { return m_Blob.size(); }
        virtual ~ShaderProgram();

    private:
        Blob m_Blob;
    };
}
