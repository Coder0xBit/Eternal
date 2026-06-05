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
        Blob& getBlob() { return mBlob; }
        size_t getBlobSize() const { return mBlob.size(); }
        ~ShaderProgram() override;

    private:
        Blob mBlob;
    };
}
