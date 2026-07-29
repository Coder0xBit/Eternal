#pragma once

#include "utils/Base.h"
#include "core/scene/TransformComponent.h"
#include "core/scene/HierarchyComponent.h"
#include "core/resource/Model.h"

#include <entt/entt.hpp>

namespace Vortak {
    class Entity;

    struct TestEntityDetails {
        std::string name = "test_entity";
        std::string filePath = "res/models/cube.obj";
        std::string texturePath = "";
        glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    class Scene {
    public:
        Scene() = default;

        ~Scene() = default;

        Entity createEntity(std::string name);

        void addEntity(TestEntityDetails testEntity);

        void setParent(Entity child, Entity parent);

        HierarchyComponent& getOrAddHierarchy(Entity entity);

        template <typename... Components>
        auto getAllEntityWith() {
            return mRegistry.view<Components...>();
        }

        template <typename T>
        void onComponentAdded(std::function<void(Entity&, T&)> callback) {
            auto typeIndex = std::type_index(typeid(T));

            auto callbackFunction = [callback](Entity& entity, void* component) {
                callback(entity, *static_cast<T*>(component));
            };

            mComponentAddedObservers[typeIndex].emplace_back(callbackFunction);
        }

    private:
        template <typename T>
        void notifyObservers(Entity& entity, T& component) {
            auto it = mComponentAddedObservers.find(std::type_index(typeid(T)));
            if (it != mComponentAddedObservers.end()) {
                for (const auto& observer : it->second) {
                    observer(entity, &component);
                }
            }
        }

        entt::registry mRegistry;

        using ObserverFunction = std::function<void(Entity&, void*)>;
        std::unordered_map<std::type_index, std::vector<ObserverFunction>> mComponentAddedObservers;

        friend class Entity;
    };
}
