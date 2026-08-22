#include "core/graphics/opengl/OpenGLPlatform.h"
#include "core/graphics/opengl/OpenGLShader.h"

#include "core/resource/ResourceManager.h"

namespace Vortak {
    OpenGLPlatform::OpenGLPlatform() {
        if (!gladLoadGL(glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    void OpenGLPlatform::initialize() {}

    void OpenGLPlatform::shutDown() {}

    SwapChain* OpenGLPlatform::createSwapChain(Window* window) {
        return nullptr;
    }

    OpenGLPlatform::~OpenGLPlatform() {}
}
