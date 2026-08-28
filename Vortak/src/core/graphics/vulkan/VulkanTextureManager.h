#pragma once

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanTexture.h"
#include "core/graphics/vulkan/VulkanDevice.h"
#include "core/scene/Scene.h"
#include "core/scene/MaterialComponent.h"

namespace Vortak {

	class VulkanTextureManager {
	public:
		using EntityId = uint32_t;
		using VulkanTextureContainer = std::unordered_map<EntityId, std::shared_ptr<VulkanTexture>>;

		VulkanTextureManager(Vortak::VulkanDevice* vulkanPlatform, Scene* scene);
		~VulkanTextureManager();

		void addTexture(EntityId entityId, const MaterialComponent& materialComponent);

		std::shared_ptr<VulkanTexture> getTexture(EntityId entityId) const {
			if (auto it = mTextures.find(entityId); it != mTextures.end()) {
				return it->second;
			}
			return nullptr;
		}

		const VulkanTextureContainer& getTextures() const { return mTextures; }

	private:
		void initialize();
		void initializeTexture(std::shared_ptr<VulkanTexture> vulkanTexture) const;
		void createCommandPool();

		vk::CommandPool mCommandPool;

		Vortak::VulkanDevice* mVulkanPlatform = nullptr;
		Scene* mScene = nullptr;
		VulkanTextureContainer mTextures;
	};
}