#pragma once
#include "core/graphics/GraphicsDevice.h"
#include "core/graphics/Shader.h"
#include "core/graphics/ShaderType.h"

namespace Vortak {
    class OpenGLDevice : public GraphicsDevice {
    public:
        OpenGLDevice();

        void initialize() override;

        void shutDown() override;

        SwapChain* createSwapChain(Window* window) override;

        ~OpenGLDevice() override;
    };
}
