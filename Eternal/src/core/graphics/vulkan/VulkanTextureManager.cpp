#include "core/graphics/vulkan/VulkanTextureManager.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/Entity.h"

namespace Eternal {
    VulkanTextureManager::VulkanTextureManager(Eternal::VulkanPlatform* vulkanPlatform, Scene* scene)
        : m_VulkanPlatform(vulkanPlatform), m_Scene(scene) {
        createCommandPool();
        initialize();
    }

    void VulkanTextureManager::initialize() {
        for (auto& e: m_Scene->getAllEntityWith<Eternal::MeshComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene);
            EntityId entityUUID = entity.getUUID();

            if (m_Textures.contains(entityUUID) && m_Textures[entityUUID] != nullptr)
                continue;

            if (auto component = entity.tryGetComponent<Eternal::MaterialComponent>()) {
                addTexture(entityUUID, *component);
            }
        }
    }

    VulkanTextureManager::~VulkanTextureManager() {
        m_VulkanPlatform->destroyCommandPool(m_CommandPool);
    }

    void VulkanTextureManager::addTexture(EntityId entityId, const MaterialComponent& materialComponent) {
        const Eternal::Image* albedoTextureImage = materialComponent.getAlbedoTexture();
        if (albedoTextureImage == nullptr) {
            m_Textures[entityId] = nullptr;
            return;
        }

        std::shared_ptr<VulkanTexture> vulkanTexture =
                std::make_shared<VulkanTexture>(m_VulkanPlatform, albedoTextureImage);

        initializeTexture(vulkanTexture);

        m_Textures[entityId] = vulkanTexture;
    }

    void VulkanTextureManager::initializeTexture(std::shared_ptr<VulkanTexture> vulkanTexture) const {
        vk::Queue graphicsQueue = m_VulkanPlatform->getGraphicsQueue();
        m_VulkanPlatform->executeOneCommand(m_CommandPool, graphicsQueue,
                                            [&](vk::CommandBuffer commandBuffer) {
                                                vulkanTexture->recordUploadCommand(commandBuffer);
                                            });
    }

    void VulkanTextureManager::createCommandPool() {
        m_CommandPool = m_VulkanPlatform->createCommandPool(vk::CommandPoolCreateFlagBits::eTransient);
    }
}
