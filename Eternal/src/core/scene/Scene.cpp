#include "Scene.h"
#include "Entity.h"

namespace Eternal {

	Entity Eternal::Scene::createEntity()
	{
		auto entity = m_Registry.create();
		return Entity(entity, this);
	}
}
