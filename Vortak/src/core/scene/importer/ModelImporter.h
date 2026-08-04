#pragma once
#include "core/scene/Entity.h"
#include "core/scene/Scene.h"
#include "core/resource/ResourceManager.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    struct ModelImporter {
        static Entity import(ResourceHandle<Model> modelHandle, Scene* scene) {
            Model* model = ResourceManager::get().get(modelHandle);

            if (!model || !model->root) {
                Logger::Error("Failed to instantiate model.");
                return {};
            }

            return processNode(
                scene,
                *model,
                *model->root,
                {},
                modelHandle);
        }

        static Entity processNode(Scene* scene, const Model& model, const ModelNode& node, Entity parent,
                                  ResourceHandle<Model> modelHandle) {
            Entity entity = scene->createEntity(node.name);

            entity.addComponent<TransformComponent>(TransformComponent::fromMatrix(node.localTransform));

            if (parent.valid()) {
                scene->setParent(entity, parent);
            }

            if (!node.meshIndices.empty()) {
                auto& meshComponent = entity.addComponent<MeshComponent>();
                meshComponent.modelHandle = modelHandle;

                meshComponent.subMeshes.reserve(node.meshIndices.size());
                for (uint32_t meshIndex : node.meshIndices) {
                    meshComponent.subMeshes.emplace_back(meshIndex);
                }
            }

            for (const auto& child : node.children) {
                processNode(scene, model, *child, entity, modelHandle);
            }

            return entity;
        }
    };
}
