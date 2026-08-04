#pragma once

#include <ranges>

#include "utils/Base.h"
#include "core/resource/Resource.h"
#include "ResourceCache.h"
#include "core/resource/Mesh.h"
#include "core/resource/Model.h"
#include "core/resource/Image.h"
#include "core/resource/ShaderProgram.h"
#include "core/resource/loader/LoaderTraits.h"

namespace Vortak {
    template <typename T>
    struct LoadedResource {
        ResourceHandle<T> handle;
        T* resource = nullptr;
    };

    class ResourceManager {
    public:
        ResourceManager(const ResourceManager&) = delete;

        ResourceManager& operator=(const ResourceManager&) = delete;

        static ResourceManager& get() {
            static ResourceManager instance;
            return instance;
        }

    public:
        template <
            typename ResourceType,
            std::enable_if_t<std::is_base_of_v<Resource<ResourceType>, ResourceType>, int> = 0
        >
        LoadedResource<ResourceType> load(const std::string& path) {
            auto& resourceCache = cache<ResourceType>();

            if (auto handle = resourceCache.find(path); handle) {
                return {handle, resourceCache.get(handle)};
            }

            typename LoaderTraits<ResourceType>::type loader;
            auto resource = loader.load(path);

            if (!resource) { return {}; }

            resource->setPath(path);

            auto handle = resourceCache.add(std::move(resource));

            return {handle, resourceCache.get(handle)};
        }

        template <
            typename ResourceType,
            std::enable_if_t<std::is_base_of_v<Resource<ResourceType>, ResourceType>, int> = 0
        >
        ResourceType* get(ResourceHandle<ResourceType> handle) {
            return cache<ResourceType>().get(handle);
        }

        template <
            typename ResourceType,
            std::enable_if_t<std::is_base_of_v<Resource<ResourceType>, ResourceType>, int> = 0
        >
        void unload(ResourceHandle<ResourceType> handle) {
            cache<ResourceType>().remove(handle);
        }

        template <
            typename ResourceType,
            std::enable_if_t<std::is_base_of_v<Resource<ResourceType>, ResourceType>, int> = 0
        >
        bool isLoaded(ResourceHandle<ResourceType> handle) const {
            return cache<ResourceType>().contains(handle);
        }

    private:
        ResourceManager() = default;

        template <typename T>
        ResourceCache<T>& cache();

        std::mutex mMutex;

        ResourceCache<Mesh> mMeshes;
        ResourceCache<Model> mModels;
        ResourceCache<Image> mImages;
        ResourceCache<ShaderProgram> mShaders;
    };
}
