#pragma once

#include <utils/Base.h>
#include <core/scene/Scene.h>
#include <core/scene/MaterialComponent.h>

#include "VulkanTexture.h"
#include "VulkanUtils.h"

namespace Eternal {
	class VulkanTextureManager {
	public:
		using EntityId = uint32_t;

		VulkanTextureManager(vk::Device device, vk::PhysicalDevice physicalDevice, Scene* scene);
		~VulkanTextureManager() = default;

		void addTexture(EntityId entityId, const MaterialComponent& materialComponent);

	private:
		void initializeTexture();

		Scene* m_Scene = nullptr;
		std::unordered_map<EntityId, std::shared_ptr<VulkanTexture>> m_Textures;

		vk::Device m_Device = nullptr;
		vk::PhysicalDevice m_PhysicalDevice = nullptr;
	};
}