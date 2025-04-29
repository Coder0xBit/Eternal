#pragma once  
#include "entt/entt.hpp"  

namespace Eternal {

	class Entity;

	class Scene {
	public:

		Scene() = default;

		~Scene() = default;

		Entity createEntity();

		template<typename... Components>
		auto getAllEntityWith() {
			return m_Registry.view<Components...>();
		}

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}