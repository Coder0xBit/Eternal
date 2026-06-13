#include "core/graphics/opengl/OpenGLShader.h"

namespace Vortak {
    OpenGLShader::OpenGLShader(ShaderProgram* shaderProgram, ShaderType shaderType)
        : Shader(shaderProgram),
          mShaderType(shaderType) {
        GLenum glShaderType = getGLShaderType(shaderType);
        mShaderID = glCreateShader(glShaderType);
        const char* shaderSource = mShaderProgram->getBlob().data();
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
