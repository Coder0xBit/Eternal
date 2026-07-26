#pragma once
#include "utils/Base.h"
#include "core/resource/Resource.h"
#include "core/resource/ResourceHandle.h"

namespace Vortak {
    template <typename T>
    class ResourceCache {
    public:
        using ResourceHandleType = ResourceHandle<T>;

        ResourceHandleType add(std::unique_ptr<T> resource) {
            ResourceHandleType handle{UUID::generate()};

            resource->setHandle(handle);

            if (!resource->getPath().empty()) {
                mPathCache.emplace(resource->getPath(), handle);
            }

            mResources.emplace(handle, std::move(resource));

            return handle;
        }

        T* get(ResourceHandleType handle) {
            auto it = mResources.find(handle);

            if (it == mResources.end())
                return nullptr;

            return it->second.get();
        }

        const T* get(ResourceHandleType handle) const {
            auto it = mResources.find(handle);

            if (it == mResources.end())
                return nullptr;

            return it->second.get();
        }

        bool contains(ResourceHandleType handle) const {
            return mResources.contains(handle);
        }

        void remove(ResourceHandleType handle) {
            auto it = mResources.find(handle);

            if (it == mResources.end())
                return;

            if (!it->second->path().empty()) {
                mPathCache.erase(it->second->path());
            }

            mResources.erase(it);
        }

        ResourceHandleType find(const std::filesystem::path& path) const {
            auto it = mPathCache.find(path);

            if (it == mPathCache.end())
                return {};

            return it->second;
        }

    private:
        std::unordered_map<ResourceHandleType, std::unique_ptr<T>> mResources;

        std::unordered_map<std::filesystem::path, ResourceHandleType> mPathCache;
    };
}
