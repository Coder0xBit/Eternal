#pragma once

#include <ranges>

#include "utils/Base.h"
#include "core/resource/Resource.h"

namespace Eternal {
    class ResourceManager {
    public:
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        static ResourceManager& get() {
            static ResourceManager instance;
            return instance;
        }

        ~ResourceManager() {
            for (auto& resourcePtr: mResources | std::views::values) {
                delete resourcePtr;
            }
            mResources.clear();
        }

    public:
        template<typename ResourceType, std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, int>  = 0>
        ResourceType* loadResource(const std::string& path) {
            auto it = mResources.find(path);
            if (it != mResources.end()) {
                return dynamic_cast<ResourceType*>(it->second);
            }

            ResourceType* resource = Memory::Allocate<ResourceType>();
            if (!resource->load(path)) {
                delete resource;
                return nullptr;
            }

            resource->setPath(path);
            mResources[path] = resource;
            return resource;
        }

        template<typename ResourceType, std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, int>  = 0>
        std::future<ResourceType*> loadResourceAsync(const std::string& path) {
            return std::async(std::launch::async,
                              [this, path]() -> ResourceType* {
                                  std::lock_guard<std::mutex> lock(mMutex);
                                  return this->loadResource<ResourceType>(path);
                              }
            );
        }

        void unloadResource(Resource* resource) {
            if (resource) {
                auto it = mResources.find(resource->getPath());
                if (it != mResources.end()) {
                    delete it->second;
                    mResources.erase(it);
                }
            }
        }

    private:
        ResourceManager() = default;

        std::unordered_map<std::string, Resource*> mResources;
        std::mutex mMutex;
    };
}
