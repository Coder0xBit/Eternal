#include "core/resource/importer/ImageImporter.h"

#include <stb_image.h>

namespace Vortak {
    std::unique_ptr<Image> ImageImporter::import(const std::filesystem::path& path) {
        auto image = std::make_unique<Image>();

        stbi_set_flip_vertically_on_load(true);

        int width;
        int height;
        int channels;

        unsigned char* data = stbi_load(
            path.string().c_str(),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha);

        if (!data) {
            Logger::Error("Failed to load image: {}", path.string());
            return nullptr;
        }

        if (channels < DESIRED_IMAGE_CHANNELS) {
            Logger::Warn(
                "Image {} has less than {} channels.",
                path.string(),
                DESIRED_IMAGE_CHANNELS);

            channels = DESIRED_IMAGE_CHANNELS;
        }

        image->data = data;
        image->width = width;
        image->height = height;
        image->channels = channels;

        return image;
    }
}
