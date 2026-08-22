#pragma once
#include "core/resource/ResourceHandle.h"
#include "core/resource/ShaderProgram.h"
#include "core/graphics/GraphicsPlatform.h"
#include "core/graphics/ShaderType.h"

namespace Vortak {
    class Shader {
    public :
        Shader(GraphicsPlatform* graphicsPlatform, ShaderProgram* shaderProgram, ShaderType shaderType)
            : mShaderProgram(shaderProgram), mGraphicsPlatform(graphicsPlatform), mShaderType(shaderType) {}

        virtual void compile() = 0;

        virtual ~Shader() = default;

    protected :
        ShaderProgram* mShaderProgram = nullptr;
        GraphicsPlatform* mGraphicsPlatform = nullptr;
        ShaderType mShaderType;
    };
}
