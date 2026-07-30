#pragma once

#include "core/resource/importer/AssetImporter.h"
#include "core/resource/Image.h"

namespace Vortak {
    class ImageImporter : public AssetImporter<Image> {
    public:
        std::unique_ptr<Image> import(const std::filesystem::path& path) override;
    };
}
