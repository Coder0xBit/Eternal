#include "core/graphics/vulkan/VulkanBufferManager.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/scene/MeshComponent.h"
#include "core/scene/Entity.h"

namespace Eternal {
    VulkanBufferManager::VulkanBufferManager(VulkanPlatform* vulkanPlatform, Scene* scene) : m_VulkanPlatform(vulkanPlatform) {
        m_Scene = scene;
        initializeBuffers();
    }

    VulkanBufferManager::~VulkanBufferManager() {
        m_VertexBuffers.clear();
        m_IndexBuffers.clear();
    }

    void VulkanBufferManager::initializeBuffers() {
        vk::MemoryPropertyFlags bufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        for (auto& e: m_Scene->getAllEntityWith<Eternal::MeshComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene);
            EntityId entityUUID = entity.getUUID();

            if (m_VertexBuffers.contains(entityUUID) || m_IndexBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Eternal::MeshComponent>();

            addBuffer(entityUUID, component);
        }

        for (auto& e: m_Scene->getAllEntityWith<Eternal::TransformComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene);
            EntityId entityUUID = entity.getUUID();

            if (m_UniformBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Eternal::TransformComponent>();

            addUniformBuffer(entityUUID, component);
        }
    }

    void VulkanBufferManager::addBuffer(EntityId entityId, const MeshComponent& renderComponent) {
        // auto vertexBuffer = std::make_shared<VulkanBuffer>(m_VulkanPlatform);
        // vertexBuffer->create(renderComponent.getVertices().size(), sizeof(Eternal::Vertex),
        //                      vk::BufferUsageFlagBits::eVertexBuffer);
        // vertexBuffer->allocate(m_BufferProperties);
        // vertexBuffer->map();
        // vertexBuffer->write((void*) (renderComponent.getVertices().data()));

        std::vector<Eternal::Vertex> vertices = renderComponent.getVertices();
        auto vertexBuffer = std::make_shared<VulkanVertexBuffer>(m_VulkanPlatform, vertices);

        m_VertexBuffers[entityId] = vertexBuffer;

        // auto indexBuffer = std::make_shared<VulkanBuffer>(m_VulkanPlatform);
        // indexBuffer->create(renderComponent.getIndices().size(), sizeof(uint32_t),
        //                     vk::BufferUsageFlagBits::eIndexBuffer);
        // indexBuffer->allocate(m_BufferProperties);
        // indexBuffer->map();
        // indexBuffer->write((void*) (renderComponent.getIndices().data()));

        std::vector<uint32_t> indices = renderComponent.getIndices();
        auto indexBuffer = std::make_shared<VulkanIndexBuffer>(m_VulkanPlatform, indices);

        m_IndexBuffers[entityId] = indexBuffer;
    }

    void VulkanBufferManager::addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent) {
        auto uniformBuffer = std::make_shared<VulkanBuffer>(m_VulkanPlatform);
        uniformBuffer->create(1, sizeof(UniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
        uniformBuffer->allocate(m_UniformBufferProperties);
        uniformBuffer->map();
        m_UniformBuffers[entityId] = uniformBuffer;
    }
}
