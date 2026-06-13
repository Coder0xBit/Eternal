#pragma once
#include "core/graphics/opengl/OpenGLUtils.h"
#include "core/graphics/Shader.h"
#include "core/graphics/ShaderType.h"

namespace Vortak {
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(ShaderProgram* shaderProgram, ShaderType shaderType);
        ~OpenGLShader() override;
        void compile() override;

    private :
        uint32_t mShaderID;
        ShaderType mShaderType;
    };
}
