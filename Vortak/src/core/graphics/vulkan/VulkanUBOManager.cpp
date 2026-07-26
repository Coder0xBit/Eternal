#include "core/graphics/vulkan/VulkanUBOManager.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/Entity.h"

namespace Vortak {
    VulkanUBOManager::VulkanUBOManager(VulkanPlatform* vulkanPlatform, Scene* scene) : mVulkanPlatform(
        vulkanPlatform) {
        mScene = scene;
        initializeBuffers();
    }

    VulkanUBOManager::~VulkanUBOManager() {
        mUniformBuffers.clear();
    }

    void VulkanUBOManager::initializeBuffers() {
        for (auto& e : mScene->getAllEntityWith<Vortak::TransformComponent>()) {
            Vortak::Entity entity = Vortak::Entity(e, mScene);
            EntityId entityUUID = entity.getUUID().value();

            if (mUniformBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Vortak::TransformComponent>();

            addUniformBuffer(entityUUID, component);
        }
    }

    void VulkanUBOManager::addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent) {
        auto uniformBuffer = std::make_shared<VulkanBuffer>(mVulkanPlatform);
        uniformBuffer->create(1, sizeof(UniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
        uniformBuffer->allocate(mUniformBufferProperties);
        uniformBuffer->map();
        mUniformBuffers[entityId] = uniformBuffer;
    }
}
