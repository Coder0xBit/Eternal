#include "core/graphics/vulkan/VulkanRenderer.h"

#include "VulkanFrameInfo.h"
#include "core/graphics/vulkan/VulkanDescsriptorSetLayout.h"
#include "core/graphics/vulkan/VulkanDescriptorPool.h"
#include "core/graphics/vulkan/VulkanImGuiOverlay.h"
#include "core/graphics/vulkan/VulkanUBOManager.h"
#include "core/graphics/vulkan/VulkanPipelineCache.h"
#include "core/scene/Entity.h"
#include "core/scene/TransformComponent.h"
#include "core/scene/MeshComponent.h"

namespace Vortak {
    VulkanRenderer::VulkanRenderer(const Builder& builder) {
        mPlatform = dynamic_cast<VulkanPlatform*>(builder->platform);
        mWindow = builder->window;
        mScene = builder->scene;

        Vortak_ASSERT(mScene != nullptr, "Scene is null");
        Vortak_ASSERT(mPlatform != nullptr, "Platform is null");
        Vortak_ASSERT(mWindow != nullptr, "Window is null");

        initialize();
    }

    VulkanRenderer::VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene) : mScene(scene),
        mPlatform(platform), mWindow(window) {
        Vortak_ASSERT(mScene != nullptr, "Scene is null");
        Vortak_ASSERT(mPlatform != nullptr, "Platform is null");
        Vortak_ASSERT(mWindow != nullptr, "Window is null");

        initialize();
    }

    void VulkanRenderer::initialize() {
        bindScene();

        auto swapchain = mPlatform->createSwapChain(mWindow);
        mVulkanSwapChain = dynamic_cast<VulkanSwapChain*>(swapchain);
        mSwapChainDetails = mVulkanSwapChain->getSwapChainDetails();

        if (!mVulkanSwapChain) {
            Vortak::Logger::Error("Failed to create Vulkan SwapChain");
            return;
        }

        mLogicalDevice = mPlatform->getLogicalDevice();

        mPhysicalDevice = mPlatform->getPhysicalDevice();

        mVulkanUBOManager = Memory::Allocate<VulkanUBOManager>(mPlatform, mScene);

        mBufferManager = Memory::Allocate<BufferManager>(mPlatform, Backend::Vulkan);

        for (const auto& [e , meshComponent]: mScene->getAllEntityWith<Vortak::MeshComponent>().each()) {
            Vortak::Entity entity = Vortak::Entity(e, mScene);
            MeshHandle meshHandle = mBufferManager->addBuffer(meshComponent);
            entity.addComponent<RenderComponent>(meshHandle);
        }

        mVulkanTextureManager = Memory::Allocate<VulkanTextureManager>(mPlatform, mScene);

        mPipelineCache = Memory::Allocate<VulkanPipelineCache>(mPlatform);

        mDescriptorPool = VulkanDescriptorPool::Builder(mLogicalDevice)
                .addPoolSize({vk::DescriptorType::eUniformBuffer, e_MaxEntities})
                .addPoolSize({vk::DescriptorType::eCombinedImageSampler, e_MaxEntities})
                .setMaxSets(e_MaxEntities)
                .build();

        mPipelineLayoutCache = Memory::Allocate<VulkanPipelineLayoutCache>(mDescriptorPool, mPlatform);

        createDepthImageView();
        createRenderPass();
        createFrameBuffers();
        initializeDescriptors();
        createCommandPool();
        createCommandBuffers();
        createSemaphores();
        createFences();
    }

    VulkanRenderer::~VulkanRenderer() {
        mLogicalDevice.waitIdle();

        for (auto semaphore: mImageAvailableSemaphores) {
            mLogicalDevice.destroySemaphore(semaphore);
        }

        for (auto semaphore: mRenderFinishedSemaphores) {
            mLogicalDevice.destroySemaphore(semaphore);
        }

        for (auto fence: mInFlightFences) {
            mLogicalDevice.destroyFence(fence);
        }

        mPlatform->destroyCommandPool(mCommandPool);

        Memory::Deallocate(mPipelineLayoutCache);
        Memory::Deallocate(mPipelineCache);
        destroyRenderPass();
        destroyDepthImageView();
        destroyFrameBuffers();
        Memory::Deallocate(mVulkanSwapChain);

        //mUniformBuffers.clear();

        mUniformDescriptorSets.clear();
        mMaterialDescriptorSets.clear();

        Memory::Deallocate(mDescriptorPool);

        Memory::Deallocate(mVulkanTextureManager);

        Memory::Deallocate(mVulkanUBOManager);
        Memory::Deallocate(mBufferManager);

        mLogicalDevice.destroy();
    }

    void VulkanRenderer::bindScene() {
        mScene->onComponentAdded<Vortak::MeshComponent>(
            [this](Vortak::Entity& entity, Vortak::MeshComponent& component) {
                MeshHandle meshHandle = mBufferManager->addBuffer(component);
                entity.addComponent<RenderComponent>(meshHandle);
            });

        mScene->onComponentAdded<Vortak::TransformComponent>(
            [this](Vortak::Entity& entity, Vortak::TransformComponent& component) {
                mVulkanUBOManager->addUniformBuffer(entity.getUUID(), component);
            });
    }

    void VulkanRenderer::updateUniformBuffers() {
    }

    void VulkanRenderer::destroyRenderPass() {
        if (mRenderPass) {
            mLogicalDevice.destroyRenderPass(mRenderPass);
            mRenderPass = nullptr;
        }
    }

    void VulkanRenderer::destroyFrameBuffers() {
        for (auto frameBuffer: mFrameBuffers) {
            mLogicalDevice.destroyFramebuffer(frameBuffer);
        }
        mFrameBuffers.clear();
    }

    void VulkanRenderer::destroyDepthImageView() {
        if (mDepthImageView) {
            mLogicalDevice.destroyImageView(mDepthImageView);
            mDepthImageView = nullptr;
        }

        if (mDepthImage) {
            mLogicalDevice.destroyImage(mDepthImage);
            mDepthImage = nullptr;
        }

        if (mDepthImageMemory) {
            mLogicalDevice.freeMemory(mDepthImageMemory);
            mDepthImageMemory = nullptr;
        }
    }

    void VulkanRenderer::createRenderPass() {
        VulkanSwapChain::SwapChainDetails swapChainDetails = mVulkanSwapChain->getSwapChainDetails();
        vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
                .setFormat(swapChainDetails.surfaceFormat.format)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference colorAttachmentReference = vk::AttachmentReference()
                .setAttachment(0)
                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentDescription depthAttachment = vk::AttachmentDescription()
                .setFormat(vk::Format::eD32Sfloat)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::AttachmentReference depthAttachmentReference = vk::AttachmentReference()
                .setAttachment(1)
                .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::SubpassDescription subPass = vk::SubpassDescription()
                .setColorAttachmentCount(1)
                .setPColorAttachments(&colorAttachmentReference)
                .setPDepthStencilAttachment(&depthAttachmentReference);

        std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo()
                .setAttachments(attachments)
                .setSubpassCount(1)
                .setPSubpasses(&subPass);

        mRenderPass = mLogicalDevice.createRenderPass(renderPassCreateInfo);
    }

    void VulkanRenderer::createDepthImageView() {
        vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
                .setImageType(vk::ImageType::e2D)
                .setFormat(vk::Format::eD32Sfloat)
                .setExtent({mSwapChainDetails.extent.width, mSwapChainDetails.extent.height, 1})
                .setMipLevels(1)
                .setArrayLayers(1)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setTiling(vk::ImageTiling::eOptimal)
                .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

        mDepthImage = mLogicalDevice.createImage(imageCreateInfo);

        vk::MemoryRequirements memRequirements = mLogicalDevice.getImageMemoryRequirements(mDepthImage);

        uint32_t memoryTypeIndex = VulkanPlatform::getMemoryType(
            mPhysicalDevice,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            memRequirements.memoryTypeBits
        );

        Vortak_ASSERT(memoryTypeIndex != 0xFFFFFFFF, "Failed to find suitable memory type for depth image");

        vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
                .setAllocationSize(memRequirements.size)
                .setMemoryTypeIndex(memoryTypeIndex);

        mDepthImageMemory = mLogicalDevice.allocateMemory(allocInfo);

        mLogicalDevice.bindImageMemory(mDepthImage, mDepthImageMemory, 0);

        vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
                .setImage(mDepthImage)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(vk::Format::eD32Sfloat)
                .setSubresourceRange({vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

        mDepthImageView = mLogicalDevice.createImageView(imageViewCreateInfo);
    }

    void VulkanRenderer::createFrameBuffers() {
        const std::vector<vk::ImageView>& swapChainImages = mVulkanSwapChain->getImageViews();
        mFrameBuffers.resize(swapChainImages.size());

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            vk::ImageView attachment[] = {swapChainImages[i], mDepthImageView};

            vk::FramebufferCreateInfo frameBufferInfo = vk::FramebufferCreateInfo()
                    .setRenderPass(mRenderPass)
                    .setAttachments(attachment)
                    .setWidth(mSwapChainDetails.extent.width)
                    .setHeight(mSwapChainDetails.extent.height)
                    .setLayers(1);

            mFrameBuffers[i] = mLogicalDevice.createFramebuffer(frameBufferInfo);
        }
    }

    void VulkanRenderer::initializeDescriptors() {
        auto uboDescriptorSetLayout = mPipelineLayoutCache->getUboDescriptorSetLayout();
        auto uniformBuffers = mVulkanUBOManager->getUniformBuffers();
        for (auto& [entityId, uniformBuffer]: uniformBuffers) {
            vk::DescriptorSet descriptorSet = mDescriptorPool->allocate(*uboDescriptorSetLayout);
            mUniformDescriptorSets[entityId] = descriptorSet;

            vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo()
                    .setBuffer(*uniformBuffer->getVkBuffer())
                    .setOffset(0)
                    .setRange(sizeof(VulkanUBOManager::UniformBuffer));

            vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
                    .setDstSet(descriptorSet)
                    .setDstBinding(0)
                    .setDstArrayElement(0)
                    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                    .setDescriptorCount(1)
                    .setPBufferInfo(&bufferInfo);
            mLogicalDevice.updateDescriptorSets(writeDescriptorSet, nullptr);
        }

        auto materialDescriptorSetLayout = mPipelineLayoutCache->getMaterialDescriptorSetLayout();
        auto textures = mVulkanTextureManager->getTextures();
        for (auto& [entityId, texture]: textures) {
            vk::DescriptorSet descriptorSet = mDescriptorPool->allocate(*materialDescriptorSetLayout);
            mMaterialDescriptorSets[entityId] = descriptorSet;

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
            mLogicalDevice.updateDescriptorSets(writeDescriptorSet, nullptr);
        }
    }

    void VulkanRenderer::createCommandPool() {
        mCommandPool = mPlatform->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    }

    void VulkanRenderer::createCommandBuffers() {
        mCommandBuffers = mPlatform->allocateCommandBuffers(mCommandPool, vk::CommandBufferLevel::ePrimary,
                                                            MAX_FRAMES_IN_FLIGHT);
    }

    void VulkanRenderer::createSemaphores() {
        mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();

            mImageAvailableSemaphores[i] = mLogicalDevice.createSemaphore(semaphoreCreateInfo);
            mRenderFinishedSemaphores[i] = mLogicalDevice.createSemaphore(semaphoreCreateInfo);
        }
    }

    void VulkanRenderer::createFences() {
        mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
            mInFlightFences[i] = mLogicalDevice.createFence(fenceCreateInfo);
        }
    }

    void VulkanRenderer::handleWindowResize() {
        if (mWindow->isMinimized())
            return;

        mVulkanSwapChain->setShouldRecreate(false);

        mLogicalDevice.waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT16_MAX);
        mLogicalDevice.resetFences(1, &mInFlightFences[mCurrentFrame]);

        mVulkanSwapChain->recreate();
        mSwapChainDetails = mVulkanSwapChain->getSwapChainDetails();
        destroyRenderPass();
        destroyDepthImageView();
        destroyFrameBuffers();
        createDepthImageView();
        createRenderPass();
        createFrameBuffers();
    }

    FrameInfo* VulkanRenderer::beginFrame() {
        if (mWindow->isMinimized())
            return nullptr;

        mVulkanSwapChain->acquire(mImageAvailableSemaphores[mCurrentFrame], &mCurrentImageIndex);

        if (mVulkanSwapChain->shouldRecreate()) {
            handleWindowResize();
            return nullptr;
        }

        mLogicalDevice.waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT16_MAX);
        mLogicalDevice.resetFences(1, &mInFlightFences[mCurrentFrame]);

        mCurrentCommandBuffer = mCommandBuffers[mCurrentFrame];
        mCurrentCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

        beginRecording(mCurrentCommandBuffer);
        return Memory::Allocate<VulkanFrameInfo>(mCurrentCommandBuffer, mCurrentImageIndex);
    }

    void VulkanRenderer::render(Vortak::Camera* camera) {
        vk::CommandBuffer commandBuffer = mCurrentCommandBuffer;

        VulkanSwapChain::SwapChainDetails swapChainDetails = mVulkanSwapChain->getSwapChainDetails();

        VulkanUBOManager::UniformBuffer sceneUbo;
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
        auto view = mScene->getAllEntityWith<TransformComponent, RenderComponent>().each();

        for (const auto& [e, transformComponent , renderComponent]: view) {
            Vortak::Entity entity = Vortak::Entity(e, mScene);

            auto transform = entity.getComponent<Vortak::TransformComponent>();
            auto material = entity.tryGetComponent<Vortak::MaterialComponent>();
            sceneUbo.model = transform.mat4();

            PipelineLayoutCacheKey pipelineLayoutKey = {
                .pipelineLayoutMask = material != nullptr
                                          ? material->getPipelineLayoutBitMask()
                                          : eDefaultPipelineLayoutBitMask,
            };

            vk::PipelineLayout pipelineLayout;
            pipelineLayout = mPipelineLayoutCache->getOrCreatePipelineLayout(pipelineLayoutKey);

            PipelineKey pipelineKey = {
                .pipelineLayoutMask = material != nullptr
                                          ? material->getPipelineLayoutBitMask()
                                          : eDefaultPipelineLayoutBitMask,
                .pipelineLayout = pipelineLayout,
                .renderPass = mRenderPass,
                .material = material,
            };

            vk::Pipeline pipeline = mPipelineCache->getOrCreate(pipelineKey);
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

            std::shared_ptr<VulkanBuffer> uniformBuffer = mVulkanUBOManager->getUniformBuffer(entity.getUUID());

            if (uniformBuffer) {
                uniformBuffer->write(&sceneUbo);
            }

            auto descriptorSet = mUniformDescriptorSets[entity.getUUID()];
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1,
                                             &descriptorSet, 0, nullptr);

            if (material) {
                auto materialDescriptorSet = mMaterialDescriptorSets[entity.getUUID()];
                commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 1, 1,
                                                 &materialDescriptorSet, 0, nullptr);
            }

            VertexBuffer* vertexBuffer = mBufferManager->getVertexBuffer(renderComponent.getMeshHandle());
            VulkanVertexBuffer* vulkanVertexBuffer = dynamic_cast<VulkanVertexBuffer*>(vertexBuffer);
            if (VulkanBuffer* vertexVulkanBuffer = vulkanVertexBuffer->getVulkanBuffer()) {
                vk::DeviceSize offset = vk::DeviceSize(0);
                commandBuffer.bindVertexBuffers(0, 1, vertexVulkanBuffer->getVkBuffer(), &offset);
            }

            IndexBuffer* indexBuffer = mBufferManager->getIndexBuffer(renderComponent.getMeshHandle());
            VulkanIndexBuffer* vulkanIndexBuffer = dynamic_cast<VulkanIndexBuffer*>(indexBuffer);
            if (VulkanBuffer* indexVulkanBuffer = vulkanIndexBuffer->getVulkanBuffer()) {
                commandBuffer.bindIndexBuffer(*(indexVulkanBuffer->getVkBuffer()), 0, vk::IndexType::eUint32);
                commandBuffer.drawIndexed(indexVulkanBuffer->getElementCount(), 1, 0, 0, 0);
            }
        }
    }

    void VulkanRenderer::endFrame() {
        vk::CommandBuffer commandBuffer = mCurrentCommandBuffer;

        endRecoding(commandBuffer);

        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

        vk::SubmitInfo submitInfo = vk::SubmitInfo()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&mImageAvailableSemaphores[mCurrentFrame])
                .setPWaitDstStageMask(waitStages)
                .setCommandBufferCount(1)
                .setPCommandBuffers(&commandBuffer)
                .setSignalSemaphoreCount(1)
                .setPSignalSemaphores(&mRenderFinishedSemaphores[mCurrentFrame]);

        auto graphicsQueue = mPlatform->getGraphicsQueue();

        vk::Result result = graphicsQueue.submit(1, &submitInfo, mInFlightFences[mCurrentFrame]);

        mVulkanSwapChain->present(mRenderFinishedSemaphores[mCurrentFrame], mCurrentImageIndex);

        if (mVulkanSwapChain->shouldRecreate()) {
            handleWindowResize();
            return;
        }

        mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::beginRecording(vk::CommandBuffer commandBuffer) {
        VulkanSwapChain::SwapChainDetails swapChainDetails = mVulkanSwapChain->getSwapChainDetails();

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
                .setRenderPass(mRenderPass)
                .setFramebuffer(mFrameBuffers[mCurrentImageIndex])
                .setRenderArea(renderArea)
                .setClearValues(clearValues);

        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    }

    void VulkanRenderer::endRecoding(vk::CommandBuffer commandBuffer) {
        commandBuffer.endRenderPass();
        commandBuffer.end();
    }
}
