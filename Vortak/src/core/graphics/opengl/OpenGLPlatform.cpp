#include "core/graphics/opengl/OpenGLPlatform.h"
#include "core/graphics/opengl/OpenGLShader.h"

#include "core/resource/ResourceManager.h"

namespace Vortak {
    OpenGLPlatform::OpenGLPlatform() {
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << glewGetErrorString(err) << '\n';
        }
    }

    void OpenGLPlatform::initialize() {
    }

    void OpenGLPlatform::shutDown() {
    }

    SwapChain* OpenGLPlatform::createSwapChain(Window* window) {
        return nullptr;
    }

    Shader* OpenGLPlatform::loadShader(const std::filesystem::path& path, ShaderType shaderType) {
        auto* shaderProgram = ResourceManager::get().loadResource<ShaderProgram>(path.string());
        OpenGLShader* shader = Memory::Allocate<OpenGLShader>(shaderProgram, shaderType);
        return shader;
    }

    OpenGLPlatform::~OpenGLPlatform() {
    }
}
