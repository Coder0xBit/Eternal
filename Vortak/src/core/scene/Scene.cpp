#include "core/scene/Scene.h"

#include "core/scene/Entity.h"
#include "core/scene/MaterialComponent.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/HierarchyComponent.h"
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

    void Scene::setParent(Entity child, Entity parent) {
        auto& childHierarchy = getOrAddHierarchy(child);
        auto& parentHierarchy = getOrAddHierarchy(parent);

        childHierarchy.parent = parent.getHandle();

        if (parentHierarchy.firstChild != entt::null) {
            parentHierarchy.firstChild = child.getHandle();
        } else {
            Entity sibling = Entity(parentHierarchy.firstChild, this);
            while (getOrAddHierarchy(sibling).nextSibling != entt::null) {
                entt::entity nextSibling = getOrAddHierarchy(sibling).nextSibling;
                sibling = Entity(nextSibling, this);
            }

            auto& siblingHierarchy = getOrAddHierarchy(sibling);

            siblingHierarchy.nextSibling = child.getHandle();
            childHierarchy.previousSibling = sibling.getHandle();
        }

        parentHierarchy.childCount++;
    }

    HierarchyComponent& Scene::getOrAddHierarchy(Entity entity) {
        if (auto* h = entity.tryGetComponent<HierarchyComponent>())
            return *h;

        return entity.addComponent<HierarchyComponent>();
    }
}
