#pragma once

#include "utils/Base.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/vulkan/VulkanBuffer.h"
#include "core/scene/Scene.h"
#include "core/scene/RenderComponent.h"
#include "core/graphics/vulkan/VulkanVertexBuffer.h"
#include "core/graphics/vulkan/VulkanIndexBuffer.h"

namespace Eternal {
    class VulkanPlatform;

    class VulkanBufferManager {
    public:
        using EntityId = uint32_t;
        using VulkanEntityData = std::unordered_map<EntityId, std::shared_ptr<VulkanBuffer>>;
        using VulkanVertexBuffers = std::unordered_map<EntityId, std::shared_ptr<VulkanVertexBuffer>>;
        using VulkanIndexBuffers = std::unordered_map<EntityId, std::shared_ptr<VulkanIndexBuffer>>;

        struct UniformBuffer {
            alignas(16) glm::mat4 projection{1.0f};
            alignas(16) glm::mat4 view{1.0f};
            alignas(16) glm::mat4 model{1.0f};
        };

        VulkanBufferManager(VulkanPlatform* vulkanPlatform, Scene* scene);

        VulkanBuffer* getVertexBuffer(EntityId entityId) {
            auto it = m_VertexBuffers.find(entityId);
            if (it != m_VertexBuffers.end()) {
                return it->second->getVulkanBuffer();
            }
            return nullptr;
        }

        VulkanBuffer* getIndexBuffer(EntityId entityId) {
            auto it = m_IndexBuffers.find(entityId);
            if (it != m_IndexBuffers.end()) {
                return it->second->getVulkanBuffer();
            }
            return nullptr;
        }

        std::shared_ptr<VulkanBuffer> getUniformBuffer(EntityId entityId) {
            auto it = m_UniformBuffers.find(entityId);
            if (it != m_UniformBuffers.end()) {
                return it->second;
            }
            return nullptr;
        }

        uint32_t getVertexBufferCount() const {
            return static_cast<uint32_t>(m_VertexBuffers.size());
        }

        uint32_t getIndexBufferCount() const {
            return static_cast<uint32_t>(m_IndexBuffers.size());
        }

        uint32_t getUniformBufferCount() const {
            return static_cast<uint32_t>(m_UniformBuffers.size());
        }

        const VulkanEntityData& getUniformBuffers() { return m_UniformBuffers; }
        const VulkanVertexBuffers& getVertexBuffers() { return m_VertexBuffers; }
        const VulkanIndexBuffers& getIndexBuffers() { return m_IndexBuffers; }

        void addBuffer(EntityId entityId, const RenderComponent& renderComponent);
        void addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent);

        ~VulkanBufferManager();

    private:
        void initializeBuffers();

        Scene* m_Scene;

        vk::MemoryPropertyFlags m_BufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        vk::MemoryPropertyFlags m_UniformBufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        VulkanPlatform* m_VulkanPlatform;

        VulkanVertexBuffers m_VertexBuffers;
        VulkanIndexBuffers m_IndexBuffers;
        VulkanEntityData m_UniformBuffers;
    };
}
