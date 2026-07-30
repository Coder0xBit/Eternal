#pragma once

#include "utils/Base.h"
#include "core/resource/Resource.h"

namespace Vortak {
    // This is basic Shader file resource class that can be loaded from a file.
    struct ShaderProgram : public Resource<ShaderProgram> {
        using Blob = std::vector<char>;
        Blob blob;
    };
}
