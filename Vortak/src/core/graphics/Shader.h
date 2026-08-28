#pragma once
#include "core/resource/ResourceHandle.h"
#include "core/resource/ShaderProgram.h"
#include "core/graphics/GraphicsDevice.h"
#include "core/graphics/ShaderType.h"

namespace Vortak {
    class Shader {
    public :
        Shader(GraphicsDevice* graphicsDevice, ShaderProgram* shaderProgram, ShaderType shaderType)
            : mShaderProgram(shaderProgram), mGraphicsDevice(graphicsDevice), mShaderType(shaderType) {}

        virtual void compile() = 0;

        virtual ~Shader() = default;

    protected :
        ShaderProgram* mShaderProgram = nullptr;
        GraphicsDevice* mGraphicsDevice = nullptr;
        ShaderType mShaderType;
    };
}
