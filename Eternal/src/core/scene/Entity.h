#pragma once
#include <utils/Base.h>
#include <utils/UUID.h>
#include "Scene.h"
#include "IdComponent.h"
#include "NameComponent.h"

#include <entt/entt.hpp>

namespace Eternal {
	class Entity {
	public:
		Entity() = default;

		explicit Entity(entt::entity entityHandle, Eternal::Scene* scene)
			: m_EntityHandle(entityHandle), m_Scene(scene) {
		}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args) {
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args) {
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& getComponent() {
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void removeComponent() {
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		UUID getUUID() {
			auto& idComponent = m_Scene->m_Registry.get<IdComponent>(m_EntityHandle);
			return idComponent.getId();
		}

		std::string getName() {
			auto& nameComponent = m_Scene->m_Registry.get<NameComponent>(m_EntityHandle);
			return nameComponent.getName();
		}

	private:

		entt::entity m_EntityHandle;

		Eternal::Scene* m_Scene = nullptr;
	};
}