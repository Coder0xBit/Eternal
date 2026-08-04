#include "core/resource/loader/ShaderLoader.h"

#include <fstream>

namespace Vortak {
    std::unique_ptr<ShaderProgram> ShaderLoader::load(
        const std::filesystem::path& path) {
        auto shader = std::make_unique<ShaderProgram>();

        std::ifstream stream(path, std::ios::binary);

        if (!stream) {
            Logger::Error("Failed to load shader {}", path.string());
            return nullptr;
        }

        stream.seekg(0, std::ios::end);
        std::streamsize size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<char> blob(size);

        stream.read(blob.data(), size);

        shader->blob = std::move(blob);

        return shader;
    }
}
