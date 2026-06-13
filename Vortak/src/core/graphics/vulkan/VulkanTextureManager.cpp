#include "core/graphics/vulkan/VulkanTextureManager.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/Entity.h"

namespace Vortak {
    VulkanTextureManager::VulkanTextureManager(Vortak::VulkanPlatform* vulkanPlatform, Scene* scene)
        : mVulkanPlatform(vulkanPlatform), mScene(scene) {
        createCommandPool();
        initialize();
    }

    void VulkanTextureManager::initialize() {
        for (auto& e: mScene->getAllEntityWith<Vortak::MeshComponent>()) {
            Vortak::Entity entity = Vortak::Entity(e, mScene);
            EntityId entityUUID = entity.getUUID();

            if (mTextures.contains(entityUUID) && mTextures[entityUUID] != nullptr)
                continue;

            if (auto component = entity.tryGetComponent<Vortak::MaterialComponent>()) {
                addTexture(entityUUID, *component);
            }
        }
    }

    VulkanTextureManager::~VulkanTextureManager() {
        mVulkanPlatform->destroyCommandPool(mCommandPool);
    }

    void VulkanTextureManager::addTexture(EntityId entityId, const MaterialComponent& materialComponent) {
        const Vortak::Image* albedoTextureImage = materialComponent.getAlbedoTexture();
        if (albedoTextureImage == nullptr) {
            mTextures[entityId] = nullptr;
            return;
        }

        std::shared_ptr<VulkanTexture> vulkanTexture =
                std::make_shared<VulkanTexture>(mVulkanPlatform, albedoTextureImage);

        initializeTexture(vulkanTexture);

        mTextures[entityId] = vulkanTexture;
    }

    void VulkanTextureManager::initializeTexture(std::shared_ptr<VulkanTexture> vulkanTexture) const {
        vk::Queue graphicsQueue = mVulkanPlatform->getGraphicsQueue();
        mVulkanPlatform->executeOneCommand(mCommandPool, graphicsQueue,
                                            [&](vk::CommandBuffer commandBuffer) {
                                                vulkanTexture->recordUploadCommand(commandBuffer);
                                            });
    }

    void VulkanTextureManager::createCommandPool() {
        mCommandPool = mVulkanPlatform->createCommandPool(vk::CommandPoolCreateFlagBits::eTransient);
    }
}
