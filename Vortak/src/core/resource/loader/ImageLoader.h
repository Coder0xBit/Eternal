#pragma once

#include "core/resource/loader/AssetLoader.h"
#include "core/resource/Image.h"

namespace Vortak {
    class ImageLoader : public AssetLoader<Image> {
    public:
        std::unique_ptr<Image> load(const std::filesystem::path& path) override;
    };
}
