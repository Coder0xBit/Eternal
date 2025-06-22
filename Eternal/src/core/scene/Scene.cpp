#include "Scene.h"
#include "Entity.h"

namespace Eternal {
	Entity Eternal::Scene::createEntity(std::string name) {
		auto entityHandle = m_Registry.create();
		Entity entity(entityHandle, this);
		entity.addComponent<Eternal::IdComponent>();
		entity.addComponent<Eternal::NameComponent>(name);
		return entity;
	}
}
