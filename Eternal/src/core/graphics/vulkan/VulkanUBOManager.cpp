#include "core/graphics/vulkan/VulkanUBOManager.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/Entity.h"

namespace Eternal {
    VulkanUBOManager::VulkanUBOManager(VulkanPlatform* vulkanPlatform, Scene* scene) : mVulkanPlatform(
        vulkanPlatform) {
        mScene = scene;
        initializeBuffers();
    }

    VulkanUBOManager::~VulkanUBOManager() {
        mUniformBuffers.clear();
    }

    void VulkanUBOManager::initializeBuffers() {
        for (auto& e: mScene->getAllEntityWith<Eternal::TransformComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, mScene);
            EntityId entityUUID = entity.getUUID();

            if (mUniformBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Eternal::TransformComponent>();

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
