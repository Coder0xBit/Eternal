#pragma once
#include "utils/Base.h"


namespace Vortak {
    template <class ResourceType>
    class AssetImporter {
    public :
        virtual ~AssetImporter() = default;

        virtual std::unique_ptr<ResourceType> import(const std::filesystem::path& path) = 0;
    };
}
