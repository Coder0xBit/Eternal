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
        auto loadedMesh = ResourceManager::get().load<Mesh>(entity.filePath);

        Vortak::Entity model = createEntity(entity.name);
        model.addComponent<Vortak::MeshComponent>(loadedMesh.handle);
        model.addComponent<Vortak::TransformComponent>(entity.initialPosition);

        if (entity.texturePath.empty())
            return;
        auto loadedTexture = ResourceManager::get().load<Image>(entity.texturePath);

        if (!loadedTexture.resource) {
            Vortak::Logger::Error("Failed to load texture from path: {}", entity.texturePath);
            model.addComponent<Vortak::MaterialComponent>(loadedTexture.resource);
            return;
        }

        model.addComponent<Vortak::MaterialComponent>(loadedTexture.resource);
    }
}
