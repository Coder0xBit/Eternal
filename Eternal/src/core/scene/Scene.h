#pragma once  
#include "entt/entt.hpp"  
#include "TransformComponent.h"

namespace Eternal {

	class Entity;

	class Scene {
	public:

		Scene() = default;
		~Scene() = default;

		Entity createEntity(std::string name);

		template<typename... Components>
		auto getAllEntityWith() {
			return m_Registry.view<Components...>();
		}

		template<typename T>
		void onComponentAdded(std::function<void(Entity, T&)> callback) {
			auto typeIndex = std::type_index(typeid(T));

			auto callbackFunction = [callback](Entity entity, void* component) {
				callback(entity, *static_cast<T*>(component));
				};

			m_ComponentAddedObservers[typeIndex].emplace_back(callbackFunction);
		}

	private:

		template<typename T>
		void notifyObservers(Entity entity, T& component) {
			auto it = m_ComponentAddedObservers.find(std::type_index(typeid(T)));
			if (it != m_ComponentAddedObservers.end()) {
				for (const auto& observer : it->second) {
					observer(entity, &component);
				}
			}
		}

		entt::registry m_Registry;

		using ObserverFunction = std::function<void(Entity, void*)>;
		std::unordered_map<std::type_index, std::vector<ObserverFunction>> m_ComponentAddedObservers;

		friend class Entity;
	};
}