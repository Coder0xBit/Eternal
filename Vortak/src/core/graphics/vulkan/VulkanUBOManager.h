#pragma once

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/scene/Scene.h"
#include "core/scene/MeshComponent.h"
#include "core/graphics/vulkan/VulkanVertexBuffer.h"
#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Vortak {
    class VulkanPlatform;

    class VulkanUBOManager {
    public:
        using EntityId = uint32_t;
        using VulkanEntityData = std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>>;
        using VulkanVertexBuffers = std::unordered_map<EntityId, std::shared_ptr<VertexBuffer>>;
        using VulkanIndexBuffers = std::unordered_map<EntityId, std::shared_ptr<IndexBuffer>>;

        struct UniformBuffer {
            alignas(16) glm::mat4 projection{1.0f};
            alignas(16) glm::mat4 view{1.0f};
            alignas(16) glm::mat4 model{1.0f};
        };

        VulkanUBOManager(VulkanPlatform* vulkanPlatform, Scene* scene);

        std::shared_ptr<VulkanBuffer> getUniformBuffer(EntityId entityId) {
            auto it = mUniformBuffers.find(entityId);
            if (it != mUniformBuffers.end()) {
                return it->second;
            }
            return nullptr;
        }

        uint32_t getUniformBufferCount() const {
            return static_cast<uint32_t>(mUniformBuffers.size());
        }

        const VulkanEntityData& getUniformBuffers() { return mUniformBuffers; }

        void addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent);

        ~VulkanUBOManager();

    private:
        void initializeBuffers();

        Scene* mScene;

        vk::MemoryPropertyFlags mUniformBufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        VulkanPlatform* mVulkanPlatform;
        VulkanEntityData mUniformBuffers;
    };
}
