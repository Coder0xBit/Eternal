#include "core/graphics/opengl/OpenGLDevice.h"
#include "core/graphics/opengl/OpenGLShader.h"

#include "core/resource/ResourceManager.h"

namespace Vortak {
    OpenGLDevice::OpenGLDevice() {
        if (!gladLoadGL(glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    void OpenGLDevice::initialize() {}

    void OpenGLDevice::shutDown() {}

    SwapChain* OpenGLDevice::createSwapChain(Window* window) {
        return nullptr;
    }

    OpenGLDevice::~OpenGLDevice() {}
}
