#pragma once
#include "core/resource/ResourceHandle.h"
#include "core/resource/ShaderProgram.h"

namespace Vortak {
    class Shader {
    public :
        Shader(ShaderProgram* shaderProgram) : mShaderProgram(shaderProgram) {
        }

        virtual void compile() = 0;

        virtual ~Shader() = default;

    protected :
        ShaderProgram* mShaderProgram = nullptr;
    };
}
