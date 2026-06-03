#pragma once

#include "utils/Base.h"
#include "utils/UUID.h"
#include "core/scene/Scene.h"
#include "core/scene/IdComponent.h"
#include "core/scene/NameComponent.h"

namespace Eternal {
    class Entity {
    public:
        Entity() = default;

        explicit Entity(entt::entity entityHandle, Eternal::Scene* scene)
            : mEntityHandle(entityHandle), mScene(scene) {
        }

        template<typename T, typename... Args>
        T& addComponent(Args&&... args) {
            T& component = mScene->mRegistry.emplace<T>(mEntityHandle, std::forward<Args>(args)...);
            mScene->notifyObservers<T>(*this, component);
            return component;
        }

        template<typename T, typename... Args>
        T& addOrReplaceComponent(Args&&... args) {
            T& component = mScene->mRegistry.emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
            mScene->notifyObservers<T>(*this, component);
            return component;
        }

        template<typename T>
        T& getComponent() {
            return mScene->mRegistry.get<T>(mEntityHandle);
        }

        template<typename T>
        T* tryGetComponent() {
            return mScene->mRegistry.try_get<T>(mEntityHandle);
        }

        template<typename T>
        void removeComponent() {
            mScene->mRegistry.remove<T>(mEntityHandle);
        }

        UUID getUUID() {
            auto& idComponent = mScene->mRegistry.get<IdComponent>(mEntityHandle);
            return idComponent.getId();
        }

        std::string getName() {
            auto& nameComponent = mScene->mRegistry.get<NameComponent>(mEntityHandle);
            return nameComponent.getName();
        }

    private:
        entt::entity mEntityHandle;
        Eternal::Scene* mScene = nullptr;
    };
}
