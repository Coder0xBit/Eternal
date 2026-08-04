#pragma once

#include "core/resource/loader/AssetLoader.h"
#include "core/resource/ShaderProgram.h"

namespace Vortak {
    class ShaderLoader : public AssetLoader<ShaderProgram> {
    public:
        std::unique_ptr<ShaderProgram> load(const std::filesystem::path& path) override;
    };
}
