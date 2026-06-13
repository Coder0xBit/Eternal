#pragma once
#include "core/graphics/GraphicsPlatform.h"
#include "core/graphics/Shader.h"
#include "core/graphics/ShaderType.h"

namespace Vortak {
    class OpenGLPlatform : public GraphicsPlatform {
    public:
        OpenGLPlatform();
        void initialize() override;
        void shutDown() override;
        SwapChain* createSwapChain(Window* window) override;
        Shader* loadShader(const std::filesystem::path& path, ShaderType shaderType);
        ~OpenGLPlatform() override;
    };
}
