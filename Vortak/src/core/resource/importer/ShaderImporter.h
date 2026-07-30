#pragma once

#include "core/resource/importer/AssetImporter.h"
#include "core/resource/ShaderProgram.h"

namespace Vortak {
    class ShaderImporter : public AssetImporter<ShaderProgram> {
    public:
        std::unique_ptr<ShaderProgram> import(const std::filesystem::path& path) override;
    };
}
