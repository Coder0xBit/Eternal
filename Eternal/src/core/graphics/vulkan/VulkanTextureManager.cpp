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
	}

	void VulkanTextureManager::initializeTexture(std::shared_ptr<VulkanTexture> vulkanTexture) {
		vk::Queue graphicsQueue = m_VulkanPlatform->getGraphicsQueue();
		vk::Format imageFormat = vulkanTexture->getFormat();

		vk::CommandBuffer commandBuffer = m_VulkanPlatform->beginSingleCommand(m_CommandPool);

		{
			VulkanTexture::LayoutTransitionInfo layoutTransitionInfo = vulkanTexture->getLayoutTransitionInfo(imageFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
			commandBuffer.pipelineBarrier(layoutTransitionInfo.sourceStage, layoutTransitionInfo.destinationStage, {}, {}, {}, { layoutTransitionInfo.imageMemoryBarrier });
		}

		{
			vk::BufferImageCopy region = vulkanTexture->getRegionForCopy();
			auto stagingBuffer = vulkanTexture->getStagingBuffer()->getBuffer();
			commandBuffer.copyBufferToImage(*stagingBuffer, vulkanTexture->getImage(), vk::ImageLayout::eTransferDstOptimal, region);
		}

		{
			VulkanTexture::LayoutTransitionInfo layoutTransitionInfo = vulkanTexture->getLayoutTransitionInfo(imageFormat, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
			commandBuffer.pipelineBarrier(layoutTransitionInfo.sourceStage, layoutTransitionInfo.destinationStage, {}, {}, {}, { layoutTransitionInfo.imageMemoryBarrier });
		}

		m_VulkanPlatform->endSingleCommand(m_CommandPool, commandBuffer, graphicsQueue);
	}

	void VulkanTextureManager::createCommandPool() {
		m_CommandPool = m_VulkanPlatform->createCommandPool(vk::CommandPoolCreateFlagBits::eTransient);
	}
}


