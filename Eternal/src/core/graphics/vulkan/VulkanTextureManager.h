#pragma once

#include <utils/Base.h>
#include <core/scene/Scene.h>
#include <core/scene/MaterialComponent.h>

#include "VulkanTexture.h"
#include "VulkanUtils.h"
#include "VulkanPlatform.h"

namespace Eternal {
	class VulkanTextureManager {
	public:
		using EntityId = uint32_t;

		VulkanTextureManager(Eternal::VulkanPlatform* vulkanPlatform, Scene* scene);
		~VulkanTextureManager();

		void addTexture(EntityId entityId, const MaterialComponent& materialComponent);

		std::shared_ptr<VulkanTexture> getTexture(EntityId entityId) const {
			auto it = m_Textures.find(entityId);
			if (it != m_Textures.end()) {
				return it->second;
			}
			return nullptr;
		}

	private:
		void initialize();
		void initializeTexture(std::shared_ptr<VulkanTexture> vulkanTexture);
		void createCommandPool();

		vk::CommandPool m_CommandPool;

		Eternal::VulkanPlatform* m_VulkanPlatform = nullptr;
		Scene* m_Scene = nullptr;
		vk::Device m_Device = nullptr;
		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		std::unordered_map<EntityId, std::shared_ptr<VulkanTexture>> m_Textures;

	};
}