#pragma once

#include <eternal/utils/Base.h>
#include <eternal/core/ecs/Component.h>
#include <eternal/core/ecs/Entity.h>

namespace Eternal {
	class EntityManager {
	public:

		using ComponentTypeIndex = std::type_index;

		using EntityId = uint32_t;

		EntityManager() = default;

		~EntityManager() = default;

		Entity createEntity()
		{
			EntityId id = m_NextEntityId++;
			Entity entity = Entity(id);
			return entity;
		}

		template<typename T, typename ... Args>
		void addComponent(Entity& entity, Args&&... args)
		{
			addComponent<T>(entity.getId(), std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent(Entity& entity)
		{
			return getComponent(entity.getId());
		}

		template<typename T>
		void removeComponent(Entity& entity)
		{
			removeComponent(entity.getId());
		}

		template<typename T>
		bool hasComponent(Entity& entity)
		{
			return hasComponent(entity.getId());
		}

		template<typename T>
		T& getComponent(uint32_t entityId)
		{
			auto& storage = getComponentStorage<T>();
			auto it = storage.find(entityId);
			ETERNAL_ASSERT(it != storage.end(), "Component not found for entity");
			return it->second;
		}

		template<typename T, typename ... Args>
		void addComponent(uint32_t entityId, Args&&... args)
		{
			auto& storage = getComponentStorage<T>();
			storage[entityId] = T(std::forward<Args>(args)...);
		}

		template<typename T>
		void removeComponent(uint32_t entityId)
		{
			auto& storage = getComponentStorage<T>();
			auto it = storage.find(entityId);
			ETERNAL_ASSERT(it != storage.end(), "Component not found for entity");
			storage.erase(it);
		}

		template<typename T>
		bool hasComponent(uint32_t entityId)
		{
			auto& storage = getComponentStorage<T>();
			auto it = storage.find(entityId);
			return it != storage.end();
		}

		template<typename T>
		std::unordered_map<EntityId, T>& getComponentStorage()
		{
			auto it = m_ComponentStorage.find(typeid(T));
			if (it == m_ComponentStorage.end()) {
				auto storage = std::make_unique<ComponentStorage<T>>();
				auto& ref = static_cast<ComponentStorage<T>*>(storage.get())->components;
				m_ComponentStorage[typeid(T)] = std::move(storage);
				return ref;
			}
			return static_cast<ComponentStorage<T>*>(it->second.get())->components;
		}

	private:
		EntityId m_NextEntityId = 0;

		struct ComponentStorageBase {
			virtual ~ComponentStorageBase() = default;
		};

		template<typename T>
		struct ComponentStorage : public ComponentStorageBase {
			std::unordered_map<EntityId, T> components;
		};

		std::unordered_map<ComponentTypeIndex, std::unique_ptr<ComponentStorageBase>> m_ComponentStorage;
	};
}