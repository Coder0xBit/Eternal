#include "core/graphics/vulkan/VulkanBufferManager.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/Entity.h"

namespace Eternal {
    VulkanBufferManager::VulkanBufferManager(VulkanPlatform* vulkanPlatform, Scene* scene) : mVulkanPlatform(
        vulkanPlatform) {
        mScene = scene;
        initializeBuffers();
    }

    VulkanBufferManager::~VulkanBufferManager() {
        mVertexBuffers.clear();
        mIndexBuffers.clear();
    }

    void VulkanBufferManager::initializeBuffers() {
        vk::MemoryPropertyFlags bufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        for (auto& e: mScene->getAllEntityWith<Eternal::MeshComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, mScene);
            EntityId entityUUID = entity.getUUID();

            if (mVertexBuffers.contains(entityUUID) || mIndexBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Eternal::MeshComponent>();

            addBuffer(entityUUID, component);
        }

        for (auto& e: mScene->getAllEntityWith<Eternal::TransformComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, mScene);
            EntityId entityUUID = entity.getUUID();

            if (mUniformBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Eternal::TransformComponent>();

            addUniformBuffer(entityUUID, component);
        }
    }

    void VulkanBufferManager::addBuffer(EntityId entityId, const MeshComponent& renderComponent) {
        std::vector<Eternal::Vertex> vertices = renderComponent.getVertices();
        auto vertexBuffer = VertexBuffer::Builder()
                .graphicsPlatform(mVulkanPlatform)
                .backend(Backend::Vulkan)
                .attribute({0, VertexAttribute::POSITON, ElementType::FLOAT3, sizeof(float) * 3, false})
                .attribute({1, VertexAttribute::COLOR, ElementType::FLOAT3, sizeof(float) * 3, false})
                .attribute({2, VertexAttribute::NORMAL, ElementType::FLOAT3, sizeof(float) * 3, false})
                .attribute({3, VertexAttribute::UV, ElementType::FLOAT3, sizeof(float) * 2, false})
                .build();

        vertexBuffer->setBuffer(vertices);
        mVertexBuffers[entityId] = std::move(vertexBuffer);

        std::vector<uint32_t> indices = renderComponent.getIndices();
        auto indexBuffer = std::make_shared<VulkanIndexBuffer>(mVulkanPlatform, indices);

        mIndexBuffers[entityId] = indexBuffer;
    }

    void VulkanBufferManager::addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent) {
        auto uniformBuffer = std::make_shared<VulkanBuffer>(mVulkanPlatform);
        uniformBuffer->create(1, sizeof(UniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
        uniformBuffer->allocate(mUniformBufferProperties);
        uniformBuffer->map();
        mUniformBuffers[entityId] = uniformBuffer;
    }
}
