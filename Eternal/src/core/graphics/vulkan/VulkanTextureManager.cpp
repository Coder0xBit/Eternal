#include "VulkanTextureManager.h"

#include <core/scene/Entity.h>

namespace Eternal {

	VulkanTextureManager::VulkanTextureManager(Eternal::VulkanPlatform* vulkanPlatform, Scene* scene)
		: m_VulkanPlatform(vulkanPlatform), m_Scene(scene) {
		m_Device = m_VulkanPlatform->getLogicalDevice();
		m_PhysicalDevice = m_VulkanPlatform->getPhysicalDevice();
		createCommandPool();
	}

	void VulkanTextureManager::initialize() {
		for (auto& e : m_Scene->getAllEntityWith<Eternal::MaterialComponent>()) {
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);
			EntityId entityUUID = entity.getUUID();

			if (m_Textures.count(entityUUID) >= 1)
				continue;

			auto& component = entity.getComponent<Eternal::MaterialComponent>();
			addTexture(entityUUID, component);
		}
	}

	VulkanTextureManager::~VulkanTextureManager() {
		m_VulkanPlatform->destroyCommandPool(m_CommandPool);
	}

	void VulkanTextureManager::addTexture(EntityId entityId, const MaterialComponent& materialComponent) {
		const Eternal::Image* albedoTextureImage = materialComponent.getAlbedoTexture();
		std::shared_ptr<VulkanTexture> vulkanTexture =
			std::make_shared<VulkanTexture>(m_Device, m_PhysicalDevice, albedoTextureImage);

		initializeTexture(vulkanTexture);

		m_Textures[entityId] = vulkanTexture;
	}

	void VulkanTextureManager::initializeTexture(std::shared_ptr<VulkanTexture> vulkanTexture) {
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


