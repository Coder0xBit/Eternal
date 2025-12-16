#include "core/graphics/vulkan/VulkanRenderer.h"

#include "VulkanFrameInfo.h"
#include "core/graphics/vulkan/VulkanDescsriptorSetLayout.h"
#include "core/graphics/vulkan/VulkanDescriptorPool.h"
#include "core/graphics/vulkan/VulkanImGuiOverlay.h"
#include "core/graphics/vulkan/VulkanBufferManager.h"
#include "core/graphics/vulkan/VulkanPipelineCache.h"
#include "core/scene/Entity.h"
#include "core/scene/TransformComponent.h"
#include "core/scene/RenderComponent.h"

namespace Eternal {
    VulkanRenderer::VulkanRenderer(const Builder& builder) {
        m_Platform = dynamic_cast<VulkanPlatform*>(builder->platform);
        m_Window = builder->window;
        m_Scene = builder->scene;

        ETERNAL_ASSERT(m_Scene != nullptr, "Scene is null");
        ETERNAL_ASSERT(m_Platform != nullptr, "Platform is null");
        ETERNAL_ASSERT(m_Window != nullptr, "Window is null");

        initialize();
    }

    VulkanRenderer::VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene) : m_Scene(scene),
        m_Platform(platform), m_Window(window) {
        ETERNAL_ASSERT(m_Scene != nullptr, "Scene is null");
        ETERNAL_ASSERT(m_Platform != nullptr, "Platform is null");
        ETERNAL_ASSERT(m_Window != nullptr, "Window is null");

        initialize();
    }

    void VulkanRenderer::initialize() {
        bindScene();

        auto swapchain = m_Platform->createSwapChain(m_Window);
        m_VulkanSwapChain = dynamic_cast<VulkanSwapChain*>(swapchain);

        if (!m_VulkanSwapChain) {
            Eternal::Logger::Error("Failed to create Vulkan SwapChain");
            return;
        }

        m_RenderPass = m_VulkanSwapChain->getRenderPass();
        ETERNAL_ASSERT(m_RenderPass, "Render pass is null");

        m_LogicalDevice = m_Platform->getLogicalDevice();

        m_PhysicalDevice = m_Platform->getPhysicalDevice();

        m_VulkanBufferManager = Memory::Allocate<VulkanBufferManager>(m_LogicalDevice, m_PhysicalDevice, m_Scene);
        m_VulkanTextureManager = Memory::Allocate<VulkanTextureManager>(m_Platform, m_Scene);

        m_PipelineCache = Memory::Allocate<VulkanPipelineCache>(m_Platform);

        m_DescriptorPool = VulkanDescriptorPool::Builder(m_LogicalDevice)
                .addPoolSize({vk::DescriptorType::eUniformBuffer, e_MaxEntities})
                .addPoolSize({vk::DescriptorType::eCombinedImageSampler, e_MaxEntities})
                .setMaxSets(e_MaxEntities)
                .build();

        m_PipelineLayoutCache = Memory::Allocate<VulkanPipelineLayoutCache>(m_DescriptorPool, m_Platform);

        initializeDescriptors();
        createCommandPool();
        createCommandBuffers();
        createSemaphores();
        createFences();
    }

    VulkanRenderer::~VulkanRenderer() {
        m_LogicalDevice.waitIdle();

        for (auto semaphore: m_ImageAvailableSemaphores) {
            m_LogicalDevice.destroySemaphore(semaphore);
        }

        for (auto semaphore: m_RenderFinishedSemaphores) {
            m_LogicalDevice.destroySemaphore(semaphore);
        }

        for (auto fence: m_InFlightFences) {
            m_LogicalDevice.destroyFence(fence);
        }

        m_Platform->destroyCommandPool(m_CommandPool);

        Memory::Deallocate(m_PipelineLayoutCache);
        Memory::Deallocate(m_PipelineCache);
        Memory::Deallocate(m_VulkanSwapChain);

        //m_UniformBuffers.clear();

        m_UniformDescriptorSets.clear();
        m_MaterialDescriptorSets.clear();

        Memory::Deallocate(m_DescriptorPool);

        Memory::Deallocate(m_VulkanTextureManager);

        Memory::Deallocate(m_VulkanBufferManager);

        m_LogicalDevice.destroy();
    }

    void VulkanRenderer::bindScene() {
        m_Scene->onComponentAdded<Eternal::RenderComponent>(
            [this](Eternal::Entity& entity, Eternal::RenderComponent& component) {
                m_VulkanBufferManager->addBuffer(entity.getUUID(), component);
            });

        m_Scene->onComponentAdded<Eternal::TransformComponent>(
            [this](Eternal::Entity& entity, Eternal::TransformComponent& component) {
                m_VulkanBufferManager->addUniformBuffer(entity.getUUID(), component);
            });
    }

    void VulkanRenderer::updateUniformBuffers() {
    }

    void VulkanRenderer::initializeDescriptors() {
        auto uboDescriptorSetLayout = m_PipelineLayoutCache->getUboDescriptorSetLayout();
        auto uniformBuffers = m_VulkanBufferManager->getUniformBuffers();
        for (auto& [entityId, uniformBuffer]: uniformBuffers) {
            vk::DescriptorSet descriptorSet = m_DescriptorPool->allocate(*uboDescriptorSetLayout);
            m_UniformDescriptorSets[entityId] = descriptorSet;

            vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo()
                    .setBuffer(*uniformBuffer->getBuffer())
                    .setOffset(0)
                    .setRange(sizeof(VulkanBufferManager::UniformBuffer));

            vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
                    .setDstSet(descriptorSet)
                    .setDstBinding(0)
                    .setDstArrayElement(0)
                    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                    .setDescriptorCount(1)
                    .setPBufferInfo(&bufferInfo);
            m_LogicalDevice.updateDescriptorSets(writeDescriptorSet, nullptr);
        }

        auto materialDescriptorSetLayout = m_PipelineLayoutCache->getMaterialDescriptorSetLayout();
        auto textures = m_VulkanTextureManager->getTextures();
        for (auto& [entityId, texture]: textures) {
            vk::DescriptorSet descriptorSet = m_DescriptorPool->allocate(*materialDescriptorSetLayout);
            m_MaterialDescriptorSets[entityId] = descriptorSet;

            vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo()
                    .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setImageView(texture->getImageView())
                    .setSampler(texture->getSampler());

            vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
                    .setDstSet(descriptorSet)
                    .setDstBinding(0)
                    .setDstArrayElement(0)
                    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                    .setDescriptorCount(1)
                    .setPImageInfo(&imageInfo);
            m_LogicalDevice.updateDescriptorSets(writeDescriptorSet, nullptr);
        }
    }

    void VulkanRenderer::createCommandPool() {
        m_CommandPool = m_Platform->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    }

    void VulkanRenderer::createCommandBuffers() {
        m_CommandBuffers = m_Platform->allocateCommandBuffers(m_CommandPool, vk::CommandBufferLevel::ePrimary,
                                                              MAX_FRAMES_IN_FLIGHT);
    }

    void VulkanRenderer::createSemaphores() {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();

            m_ImageAvailableSemaphores[i] = m_LogicalDevice.createSemaphore(semaphoreCreateInfo);
            m_RenderFinishedSemaphores[i] = m_LogicalDevice.createSemaphore(semaphoreCreateInfo);
        }
    }

    void VulkanRenderer::createFences() {
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
            m_InFlightFences[i] = m_LogicalDevice.createFence(fenceCreateInfo);
        }
    }

    void VulkanRenderer::handleWindowResize() {
        if (m_Window->isMinimized())
            return;

        m_VulkanSwapChain->setShouldRecreate(false);

        m_LogicalDevice.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT16_MAX);
        m_LogicalDevice.resetFences(1, &m_InFlightFences[m_CurrentFrame]);

        m_VulkanSwapChain->recreate();
        m_RenderPass = m_VulkanSwapChain->getRenderPass();
    }

    FrameInfo* VulkanRenderer::beginFrame() {
        if (m_Window->isMinimized())
            return nullptr;

        m_VulkanSwapChain->acquire(m_ImageAvailableSemaphores[m_CurrentFrame], &m_CurrentImageIndex);

        if (m_VulkanSwapChain->shouldRecreate()) {
            handleWindowResize();
            return nullptr;
        }

        m_LogicalDevice.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT16_MAX);
        m_LogicalDevice.resetFences(1, &m_InFlightFences[m_CurrentFrame]);

        m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentFrame];
        m_CurrentCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

        beginRecording(m_CurrentCommandBuffer);
        return Memory::Allocate<VulkanFrameInfo>(m_CurrentCommandBuffer, m_CurrentImageIndex);
    }

    void VulkanRenderer::render(Eternal::Camera* camera) {
        vk::CommandBuffer commandBuffer = m_CurrentCommandBuffer;

        VulkanSwapChain::SwapChainDetails swapChainDetails = m_VulkanSwapChain->getSwapChainDetails();

        VulkanBufferManager::UniformBuffer sceneUbo;
        sceneUbo.projection = camera->getProjection();
        sceneUbo.view = camera->getView();

        vk::Viewport viewport = vk::Viewport()
                .setX(0.0f)
                .setY(0.0f)
                .setWidth(static_cast<float>(swapChainDetails.extent.width))
                .setHeight(static_cast<float>(swapChainDetails.extent.height))
                .setMinDepth(0.0f)
                .setMaxDepth(1.0f);

        commandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor = vk::Rect2D()
                .setOffset({0, 0})
                .setExtent(swapChainDetails.extent);

        commandBuffer.setScissor(0, 1, &scissor);

        for (const auto& [e, renderComponent]: m_Scene->getAllEntityWith<Eternal::RenderComponent>().each()) {
            Eternal::Entity entity = Eternal::Entity(e, m_Scene);

            auto transform = entity.getComponent<Eternal::TransformComponent>();
            auto material = entity.tryGetComponent<Eternal::MaterialComponent>();
            sceneUbo.model = transform.mat4();

            PipelineLayoutCacheKey pipelineLayoutKey = {
                .pipelineLayoutMask = material != nullptr
                                          ? material->getPipelineLayoutBitMask()
                                          : eDefaultPipelineLayoutBitMask,
            };

            vk::PipelineLayout pipelineLayout;
            pipelineLayout = m_PipelineLayoutCache->getOrCreatePipelineLayout(pipelineLayoutKey);

            PipelineKey pipelineKey = {
                .pipelineLayoutMask = material != nullptr
                                          ? material->getPipelineLayoutBitMask()
                                          : eDefaultPipelineLayoutBitMask,
                .pipelineLayout = pipelineLayout,
                .renderPass = m_VulkanSwapChain->getRenderPass(),
                .material = material,
            };

            vk::Pipeline pipeline = m_PipelineCache->getOrCreate(pipelineKey);
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

            std::shared_ptr<VulkanBuffer> uniformBuffer = m_VulkanBufferManager->getUniformBuffer(entity.getUUID());

            if (uniformBuffer) {
                uniformBuffer->write(&sceneUbo);
            }

            auto descriptorSet = m_UniformDescriptorSets[entity.getUUID()];
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1,
                                             &descriptorSet, 0, nullptr);

            if (material) {
                auto materialDescriptorSet = m_MaterialDescriptorSets[entity.getUUID()];
                commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 1, 1,
                                                 &materialDescriptorSet, 0, nullptr);
            }

            std::shared_ptr<VulkanBuffer> vertexBuffer = m_VulkanBufferManager->getVertexBuffer(entity.getUUID());
            if (vertexBuffer) {
                vk::DeviceSize offset = vk::DeviceSize(0);
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffer->getBuffer(), &offset);
            }

            std::shared_ptr<VulkanBuffer> indexBuffer = m_VulkanBufferManager->getIndexBuffer(entity.getUUID());
            if (indexBuffer) {
                commandBuffer.bindIndexBuffer(*(indexBuffer->getBuffer()), 0, vk::IndexType::eUint32);
                commandBuffer.drawIndexed(indexBuffer->getElementCount(), 1, 0, 0, 0);
            }
        }
    }

    void VulkanRenderer::endFrame() {
        vk::CommandBuffer commandBuffer = m_CurrentCommandBuffer;

        endRecoding(commandBuffer);

        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

        vk::SubmitInfo submitInfo = vk::SubmitInfo()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&m_ImageAvailableSemaphores[m_CurrentFrame])
                .setPWaitDstStageMask(waitStages)
                .setCommandBufferCount(1)
                .setPCommandBuffers(&commandBuffer)
                .setSignalSemaphoreCount(1)
                .setPSignalSemaphores(&m_RenderFinishedSemaphores[m_CurrentFrame]);

        auto graphicsQueue = m_Platform->getGraphicsQueue();

        vk::Result result = graphicsQueue.submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);

        m_VulkanSwapChain->present(m_RenderFinishedSemaphores[m_CurrentFrame], m_CurrentImageIndex);

        if (m_VulkanSwapChain->shouldRecreate()) {
            handleWindowResize();
            return;
        }

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::beginRecording(vk::CommandBuffer commandBuffer) {
        VulkanSwapChain::SwapChainDetails swapChainDetails = m_VulkanSwapChain->getSwapChainDetails();

        vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();

        commandBuffer.begin(commandBufferBeginInfo);

        vk::Rect2D renderArea = vk::Rect2D()
                .setOffset({0, 0})
                .setExtent(swapChainDetails.extent);

        vk::ClearColorValue clearColor = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
        vk::ClearValue clearValue = vk::ClearValue(clearColor);
        vk::ClearValue depthStencil = vk::ClearValue().setDepthStencil({1.0f, 0});

        std::array<vk::ClearValue, 2> clearValues = {clearValue, depthStencil};

        vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
                .setRenderPass(m_RenderPass)
                .setFramebuffer(m_VulkanSwapChain->getFrameBuffers()[m_CurrentImageIndex])
                .setRenderArea(renderArea)
                .setClearValues(clearValues);

        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    }

    void VulkanRenderer::endRecoding(vk::CommandBuffer commandBuffer) {
        commandBuffer.endRenderPass();
        commandBuffer.end();
    }
}
