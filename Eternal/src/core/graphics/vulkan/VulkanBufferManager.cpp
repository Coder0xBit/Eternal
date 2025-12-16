#include "core/graphics/vulkan/VulkanBufferManager.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/scene/RenderComponent.h"
#include "core/scene/Entity.h"

namespace Eternal {
    VulkanBufferManager::VulkanBufferManager(vk::Device device, vk::PhysicalDevice physicalDevice, Scene* scene) {
        m_Device = device;
        m_Scene = scene;
        m_PhysicalDevice = physicalDevice;

        initializeBuffers();
    }

    VulkanBufferManager::~VulkanBufferManager() {
        m_VertexBuffers.clear();
        m_IndexBuffers.clear();
    }

    void VulkanBufferManager::initializeBuffers() {
        vk::MemoryPropertyFlags bufferProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

        for (auto& e: m_Scene->getAllEntityWith<Eternal::RenderComponent>()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene);
            EntityId entityUUID = entity.getUUID();

            if (m_VertexBuffers.contains(entityUUID) || m_IndexBuffers.contains(entityUUID))
                continue;

            auto& component = entity.getComponent<Eternal::RenderComponent>();

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

    void VulkanBufferManager::addBuffer(EntityId entityId, const RenderComponent& renderComponent) {
        auto vertexBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
        vertexBuffer->create(renderComponent.getVertices().size(), sizeof(Eternal::Vertex),
                             vk::BufferUsageFlagBits::eVertexBuffer);
        vertexBuffer->allocate(m_BufferProperties);
        vertexBuffer->map();
        vertexBuffer->write((void*) (renderComponent.getVertices().data()));

        m_VertexBuffers[entityId] = vertexBuffer;

        auto indexBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
        indexBuffer->create(renderComponent.getIndices().size(), sizeof(uint32_t),
                            vk::BufferUsageFlagBits::eIndexBuffer);
        indexBuffer->allocate(m_BufferProperties);
        indexBuffer->map();
        indexBuffer->write((void*) (renderComponent.getIndices().data()));

        m_IndexBuffers[entityId] = indexBuffer;
    }

    void VulkanBufferManager::addUniformBuffer(EntityId entityId, const TransformComponent& transformComponent) {
        auto uniformBuffer = std::make_shared<VulkanBuffer>(m_Device, m_PhysicalDevice);
        uniformBuffer->create(1, sizeof(UniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
        uniformBuffer->allocate(m_UniformBufferProperties);
        uniformBuffer->map();
        m_UniformBuffers[entityId] = uniformBuffer;
    }
}
