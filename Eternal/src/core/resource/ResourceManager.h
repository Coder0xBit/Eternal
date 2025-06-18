#pragma once
#include <utils/Base.h>

#include "Resource.h"

namespace Eternal
{

	class ResourceManager {
	public:
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

		static ResourceManager& get() {
			static ResourceManager instance;
			return instance;
		}

		~ResourceManager() {
			for (auto& [path, resourcePtr] : m_Resources) {
				delete resourcePtr;
			}
			m_Resources.clear();
		}

	public:
		template<typename ResourceType, typename std::enable_if<std::is_base_of<Resource, ResourceType>::value, int>::type = 0>
		ResourceType* loadResource(const std::string& path) {
			auto it = m_Resources.find(path);
			if (it != m_Resources.end()) {
				return dynamic_cast<ResourceType*>(it->second);
			}

			ResourceType* resource = new ResourceType();
			if (!resource->load(path)) {
				delete resource;
				return nullptr;
			}

			resource->setPath(path);
			m_Resources[path] = resource;
			return resource;
		}

		template<typename ResourceType, typename std::enable_if<std::is_base_of<Resource, ResourceType>::value, int>::type = 0>
		std::future<ResourceType*>  loadResourceAsync(const std::string& path) {
			return std::async(std::launch::async,
				[this, path]() -> ResourceType* {
					std::lock_guard<std::mutex> lock(m_Mutex);
					return this->loadResource<ResourceType>(path);
				}
			);
		}

		void unloadResource(Resource* resource) {
			if (resource) {
				auto it = m_Resources.find(resource->getPath());
				if (it != m_Resources.end()) {
					delete it->second;
					m_Resources.erase(it);
				}
			}
		}

	private:
		ResourceManager() = default;

		std::unordered_map<std::string, Resource*> m_Resources;
		std::mutex m_Mutex;
	};
}