#pragma once

#include "utils/Base.h"
#include "core/log/Logger.h"

#include <GLEW/glew.h>

#include "core/graphics/ShaderType.h"

constexpr uint32_t const MAX_FRAMES_IN_FLIGHT = 2;

inline GLenum getGLShaderType(Vortak::ShaderType type) {
    switch (type) {
        case Vortak::ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
            break;
        case Vortak::ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
            break;
        default:
            return 0;
            break;
    }
}
