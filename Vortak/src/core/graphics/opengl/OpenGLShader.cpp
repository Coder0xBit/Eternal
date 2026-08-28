#include "core/graphics/opengl/OpenGLShader.h"
#include "core/resource/ResourceManager.h"

namespace Vortak {
    OpenGLShader::OpenGLShader(GraphicsDevice* graphicsDevice, ShaderProgram* shaderProgram, ShaderType shaderType)
        : Shader(graphicsDevice, shaderProgram, shaderType) {
        GLenum glShaderType = getGLShaderType(shaderType);
        mShaderID = glCreateShader(glShaderType);
        const char* shaderSource = mShaderProgram->blob.data();
        glShaderSource(mShaderID, 1, &shaderSource, nullptr);
    }

    void OpenGLShader::compile() {
        glCompileShader(mShaderID);

        int success;
        char infoLog[512];
        glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(mShaderID, 512, nullptr, infoLog);
            Vortak::Logger::Error("For {}", getGLShaderType(mShaderType));
            Vortak::Logger::Error("{}", infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteShader(mShaderID);
    }
}
