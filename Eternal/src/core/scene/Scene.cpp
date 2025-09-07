#include "Scene.h"
#include "Entity.h"
#include "MaterialComponent.h"
#include "RenderComponent.h"
#include "core/resource/Image.h"
#include "core/resource/Mesh.h"
#include "core/resource/ResourceManager.h"

namespace Eternal {
    Entity Eternal::Scene::createEntity(std::string name) {
        auto entityHandle = m_Registry.create();
        Entity entity(entityHandle, this);
        entity.addComponent<Eternal::IdComponent>();
        entity.addComponent<Eternal::NameComponent>(name);
        return entity;
    }

    void Eternal::Scene::addEntity(TestEntityDetails entity) {
        const Mesh* mesh = ResourceManager::get().loadResource<Mesh>(entity.filePath);
        if (!mesh) {
            Eternal::Logger::Error("Failed to load mesh from path: {}", entity.filePath);
            return;
        }

        Eternal::Logger::Info("Obj Vertices: {}", mesh->getVertices().size());

        Eternal::Entity model = createEntity(entity.name);
        model.addComponent<Eternal::RenderComponent>(mesh->getVertices(), mesh->getIndices());
        model.addComponent<Eternal::TransformComponent>(entity.initialPosition);

        if (!entity.texturePath.empty()) {
            Image* texture = ResourceManager::get().loadResource<Image>(entity.texturePath);
            if (!texture) {
                Eternal::Logger::Error("Failed to load texture from path: {}", entity.texturePath);
                return;
            }
            model.addComponent<Eternal::MaterialComponent>(texture);
        }
    }
}
