#pragma once
#include "utils/Base.h"


namespace Vortak {
    template <class ResourceType>
    class AssetLoader {
    public :
        virtual ~AssetLoader() = default;

        virtual std::unique_ptr<ResourceType> load(const std::filesystem::path& path) = 0;
    };
}
