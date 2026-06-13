#include "core/scene/Scene.h"
#include "core/scene/Entity.h"
#include "core/scene/MaterialComponent.h"
#include "core/scene/MeshComponent.h"
#include "core/resource/Image.h"
#include "core/resource/Mesh.h"
#include "core/resource/ResourceManager.h"

namespace Vortak {
    Entity Vortak::Scene::createEntity(std::string name) {
        auto entityHandle = mRegistry.create();
        Entity entity(entityHandle, this);
        entity.addComponent<Vortak::IdComponent>();
        entity.addComponent<Vortak::NameComponent>(name);
        return entity;
    }

    void Vortak::Scene::addEntity(TestEntityDetails entity) {
        const Mesh* mesh = ResourceManager::get().loadResource<Mesh>(entity.filePath);
        if (!mesh) {
            Vortak::Logger::Error("Failed to load mesh from path: {}", entity.filePath);
            return;
        }

        Vortak::Logger::Info("Obj Vertices: {}", mesh->getVertices().size());

        Vortak::Entity model = createEntity(entity.name);
        model.addComponent<Vortak::MeshComponent>(mesh->getVertices(), mesh->getIndices());
        model.addComponent<Vortak::TransformComponent>(entity.initialPosition);

        if (entity.texturePath.empty()) return;
        Image* texture = ResourceManager::get().loadResource<Image>(entity.texturePath);

        if (!texture) {
            Vortak::Logger::Error("Failed to load texture from path: {}", entity.texturePath);
            model.addComponent<Vortak::MaterialComponent>(texture);
            return;
        }

        model.addComponent<Vortak::MaterialComponent>(texture);
    }
}
