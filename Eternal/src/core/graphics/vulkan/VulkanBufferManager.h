#pragma once

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/scene/Scene.h"
#include "core/scene/MeshComponent.h"
#include "core/graphics/vulkan/VulkanVertexBuffer.h"
#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Eternal {
    class VulkanPlatform;

    class VulkanBufferManager {
    public:
        using EntityId = uint32_t;
        using VulkanEntityData = std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>>;
        using VulkanVertexBuffers = std::unordered_map<EntityId, std::shared_ptr<VertexBuffer>>;
        using VulkanIndexBuffers = std::unordered_map<EntityId, std::shared_ptr<VulkanIndexBuffer>>;

        struct UniformBuffer {
            alignas(16) glm::mat4 projection{1.0f};
            alignas(16) glm::mat4 view{1.0f};
            alignas(16) glm::mat4 model{1.0f};
        };

        VulkanBufferManager(VulkanPlatform* vulkanPlatform, Scene* scene);

        VulkanBuffer* getVertexBuffer(EntityId entityId) {
            auto it = mVertexBuffers.find(entityId);
            if (it != mVertexBuffers.end()) {
                VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(it->second.get());
                return vulkanVertexBuffer->getVulkanBuffer();
            }
            return nullptr;
        }

        VulkanBuffer* getIndexBuffer(EntityId entityId) {
            auto it = mIndexBuffers.find(entityId);
            if (it != mIndexBuffers.end()) {
                return it->second->getVulkanBuffer();
            }
            return nullptr;
        }

        std::shared_ptr<VulkanBuffer> getUniformBuffer(EntityId entityId) {
            auto it = mUniformBuffers.find(entityId);
            if (it != mUniformBuffers.end()) {
                return it->second;
            }
            return nullptr;
        }

        uint32_t getVertexBufferCount() const {
            return static_cast<uint32_t>(mVertexBuffers.size());
        }

        uint32_t getIndexBufferCount() const {
            return static_cast<uint32_t>(mIndexBuffers.size());
        }

        uint32_t getUniformBufferCount() const {
            return static_cast<uint32_t>(mUniformBuffers.size());
        }

        const VulkanEntityData& getUniformBuffers() { return mUniformBuffers; }
        const VulkanVertexBuffers& getVertexBuffers() { return mVertexBuffers; }
        const VulkanIndexBuffers& getIndexBuffers() { return mIndexBuffers; }

        void addBuffer(EntityId entityId, const MeshComponent& renderComponent);
        void addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent);

        ~VulkanBufferManager();

    private:
        void initializeBuffers();

        Scene* mScene;

        vk::MemoryPropertyFlags mBufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        vk::MemoryPropertyFlags mUniformBufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        VulkanPlatform* mVulkanPlatform;

        VulkanVertexBuffers mVertexBuffers;
        VulkanIndexBuffers mIndexBuffers;
        VulkanEntityData mUniformBuffers;
    };
}
