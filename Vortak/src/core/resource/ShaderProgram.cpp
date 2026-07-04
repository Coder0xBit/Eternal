#include "core/resource/ShaderProgram.h"

namespace Vortak {
    ShaderProgram::ShaderProgram() = default;

    bool ShaderProgram::load(const std::string& shaderName) {
        if (shaderName.empty()) {
            Vortak::Logger::Error("ShaderProgram::load() shaderName is empty");
            return false;
        }

        std::filesystem::path path =
                std::filesystem::path("../Vortak/res")
                / "shader"
                / "bin"
                / shaderName;

        std::ifstream stream(path, std::ios::binary);

        if (!stream) {
            Vortak::Logger::Error("ShaderProgram::load() failed");
            return false;
        }

        stream.seekg(0, std::ios_base::end);
        std::streampos size = stream.tellg();
        stream.seekg(0, std::ios_base::beg);

        mBlob.resize(size);
        stream.read(mBlob.data(), size);
        stream.close();

        return true;
    }

    ShaderProgram::~ShaderProgram() {
        mBlob.clear();
    }
}
