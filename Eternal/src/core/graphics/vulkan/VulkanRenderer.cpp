#include "VulkanRenderer.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include <core/graphics/vulkan/VulkanDescsriptorSetLayout.h>
#include <core/graphics/vulkan/VulkanDescriptorPool.h>
#include <core/scene/Entity.h>
#include <core/scene/TransformComponent.h>
#include <core/scene/RenderComponent.h>

namespace Eternal {

	VulkanRenderer::VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene) : m_Scene(scene), m_Platform(platform), m_Window(window) {
		ETERNAL_ASSERT(m_Scene != nullptr, "Scene is null");
		ETERNAL_ASSERT(m_Platform != nullptr, "Platform is null");
		ETERNAL_ASSERT(m_Window != nullptr, "Window is null");

		bindScene();

		m_Camera = Memory::Allocate<Camera>();
		float aspectRatio = m_Window->getAspectRatio();
		m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

		auto swapchain = m_Platform->createSwapChain(window);
		m_VulkanSwapChain = dynamic_cast<VulkanSwapChain*>(swapchain);

		if (!m_VulkanSwapChain) {
			Eternal::Logger::Error("Failed to create Vulkan SwapChain");
			return;
		}

		m_RenderPass = m_VulkanSwapChain->getRenderPass();
		ETERNAL_ASSERT(m_RenderPass, "Render pass is null");

		m_LogicalDevice = m_Platform->getLogicalDevice();

		m_PhysicalDevice = m_Platform->getPhysicalDevice();

		createPipeline();
		createCommandPool();
		createCommandBuffers();
		createSemaphores();
		createFences();

		m_VulkanBufferManager = Memory::Allocate<VulkanBufferManager>(m_LogicalDevice, m_PhysicalDevice, m_Scene);
		m_VulkanTextureManager = Memory::Allocate<VulkanTextureManager>(m_Platform, m_Scene);
	}

	VulkanRenderer::~VulkanRenderer() {
		m_LogicalDevice.waitIdle();

		for (auto semaphore : m_ImageAvailableSemaphores) {
			m_LogicalDevice.destroySemaphore(semaphore);
		}

		for (auto semaphore : m_RenderFinishedSemaphores) {
			m_LogicalDevice.destroySemaphore(semaphore);
		}

		for (auto fence : m_InFlightFences) {
			m_LogicalDevice.destroyFence(fence);
		}

		m_Platform->destroyCommandPool(m_CommandPool);

		Memory::Deallocate(m_VulkanPipeline);

		m_LogicalDevice.destroyPipelineLayout(m_PipelineLayout);

		Memory::Deallocate(m_VulkanSwapChain);

		//m_UniformBuffers.clear();

		//Memory::Deallocate(m_DescriptorSetLayout);

		//Memory::Deallocate(m_DescriptorPool);

		Memory::Deallocate(m_VulkanTextureManager);

		Memory::Deallocate(m_VulkanBufferManager);

		m_LogicalDevice.destroy();

		Memory::Deallocate(m_Platform);

		Memory::Deallocate(m_Camera);
	}

	void VulkanRenderer::bindScene() {
		m_Scene->onComponentAdded<Eternal::RenderComponent>([this](Eternal::Entity entity, Eternal::RenderComponent& component) {
			m_VulkanBufferManager->addBuffer(entity.getUUID(), component);
			});
	}

	void VulkanRenderer::createPipeline() {
		//createUniformBuffers();
		//initializeDescriptors();

		vk::PushConstantRange pushConstantRange = vk::PushConstantRange()
			.setOffset(0)
			.setSize(sizeof(PushConstants))
			.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(0)
			.setPushConstantRanges(pushConstantRange);

		m_PipelineLayout = m_LogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);

		vk::ShaderModule vertexShaderModule = m_Platform->loadShader(m_LogicalDevice, "res/shader/bin/vert.spv");
		vk::ShaderModule fragmentShaderModule = m_Platform->loadShader(m_LogicalDevice, "res/shader/bin/frag.spv");

		m_VulkanPipeline = Memory::Allocate<VulkanPipeline>(m_LogicalDevice);
		m_VulkanPipeline->bindLayout(m_PipelineLayout);
		m_VulkanPipeline->bindRenderPass(m_RenderPass);
		m_VulkanPipeline->bindVertexShader(vertexShaderModule);
		m_VulkanPipeline->bindFragmentShader(fragmentShaderModule);
		m_VulkanPipeline->create();

		m_LogicalDevice.destroyShaderModule(vertexShaderModule);
		m_LogicalDevice.destroyShaderModule(fragmentShaderModule);
	}

	void VulkanRenderer::createUniformBuffers() {
		vk::MemoryPropertyFlags bufferProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto buffer = std::make_shared<VulkanBuffer>(m_Platform->getLogicalDevice(), m_Platform->getPhysicalDevice());
			buffer->create(1, sizeof(UniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
			buffer->allocate(bufferProperties);
			buffer->map();
			m_UniformBuffers[i] = buffer;
		}
	}

	void VulkanRenderer::updateUniformBuffers() {
		for (auto& e : m_Scene->getAllEntityWith<Eternal::TransformComponent>()) {
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);
			auto& component = entity.getComponent<Eternal::TransformComponent>();

			UniformBuffer uniformBuffer = {};
			glm::mat4 modelMatrix = m_Camera->getProjection() * component.mat4();
			uniformBuffer.transform = modelMatrix;
			uniformBuffer.normalMatrix = component.mat4();
			m_UniformBuffers[m_CurrentFrame]->write(&uniformBuffer);
		}
	}

	void VulkanRenderer::initializeDescriptors() {
		m_DescriptorSetLayout = VulkanDescriptorSetLayout::Builder(m_LogicalDevice)
			.addBinding({ 0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex })
			.build();

		m_DescriptorPool = VulkanDescriptorPool::Builder(m_LogicalDevice)
			.addPoolSize({ vk::DescriptorType::eUniformBuffer, 1 })
			.setMaxSets(4)
			.build();

		m_DescriptorSets = m_DescriptorPool->allocate(MAX_FRAMES_IN_FLIGHT, *m_DescriptorSetLayout);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo()
				.setBuffer(*(m_UniformBuffers[i]->getBuffer()))
				.setOffset(0)
				.setRange(sizeof(UniformBuffer));

			vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
				.setDstSet(m_DescriptorSets[i])
				.setDstBinding(0)
				.setDstArrayElement(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setPBufferInfo(&bufferInfo);

			m_LogicalDevice.updateDescriptorSets(writeDescriptorSet, nullptr);
		}
	}

	void VulkanRenderer::createCommandPool() {
		m_CommandPool = m_Platform->createCommandPool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	}

	void VulkanRenderer::createCommandBuffers() {
		m_CommandBuffers = m_Platform->allocateCommandBuffers(m_CommandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
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

		m_Window->setWindowResized(false);
		m_VulkanSwapChain->setShouldRecreate(false);

		m_LogicalDevice.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT16_MAX);
		m_LogicalDevice.resetFences(1, &m_InFlightFences[m_CurrentFrame]);

		m_VulkanSwapChain->recreate();
		m_RenderPass = m_VulkanSwapChain->getRenderPass();

		float aspectRatio = m_Window->getAspectRatio();
		m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);
	}

	FrameInfo* VulkanRenderer::beginFrame() {
		if (m_Window->isMinimized())
			return nullptr;

		m_VulkanSwapChain->acquire(m_ImageAvailableSemaphores[m_CurrentFrame], &m_CurrentImageIndex);

		if (m_VulkanSwapChain->shouldRecreate() || m_Window->isResized()) {
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

	void VulkanRenderer::render() {
		VulkanSwapChain::SwapChainDetails swapChainDetails = m_VulkanSwapChain->getSwapChainDetails();

		for (auto& [e, transform] : m_Scene->getAllEntityWith<Eternal::TransformComponent>().each()) {
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);

			glm::mat4 projection = m_Camera->getProjection();
			glm::mat4 model = transform.mat4();
			glm::mat4 normal = glm::transpose(glm::inverse(transform.mat4()));

			m_PushConstants.transform = projection * model;
			m_PushConstants.normalMatrix = normal;
			m_PushConstants.modelMatrix = model;

			m_CurrentCommandBuffer.pushConstants(m_PipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstants), &m_PushConstants);

			std::shared_ptr<VulkanBuffer> vertexBuffer = m_VulkanBufferManager->getVertexBuffer(entity.getUUID());
			if (vertexBuffer) {
				vk::DeviceSize offset = vk::DeviceSize(0);
				m_CurrentCommandBuffer.bindVertexBuffers(0, 1, vertexBuffer->getBuffer(), &offset);
			}

			std::shared_ptr<VulkanBuffer> indexBuffer = m_VulkanBufferManager->getIndexBuffer(entity.getUUID());
			if (indexBuffer) {
				m_CurrentCommandBuffer.bindIndexBuffer(*(indexBuffer->getBuffer()), 0, vk::IndexType::eUint32);
			}

			vk::Viewport viewport = vk::Viewport()
				.setX(0.0f)
				.setY(0.0f)
				.setWidth((float)swapChainDetails.extent.width)
				.setHeight((float)swapChainDetails.extent.height)
				.setMinDepth(0.0f)
				.setMaxDepth(1.0f);

			m_CurrentCommandBuffer.setViewport(0, 1, &viewport);

			vk::Rect2D scissor = vk::Rect2D()
				.setOffset({ 0,0 })
				.setExtent(swapChainDetails.extent);

			m_CurrentCommandBuffer.setScissor(0, 1, &scissor);

			if (indexBuffer) {
				m_CurrentCommandBuffer.drawIndexed(indexBuffer->getElementCount(), 1, 0, 0, 0);
			}
		}
	}

	void VulkanRenderer::endFrame() {
		endRecoding(m_CurrentCommandBuffer);

		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&m_ImageAvailableSemaphores[m_CurrentFrame])
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(1)
			.setPCommandBuffers(&m_CurrentCommandBuffer)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&m_RenderFinishedSemaphores[m_CurrentFrame]);

		auto graphicsQueue = m_Platform->getGraphicsQueue();

		vk::Result result = graphicsQueue.submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);

		m_VulkanSwapChain->present(m_RenderFinishedSemaphores[m_CurrentFrame], m_CurrentImageIndex);

		if (m_VulkanSwapChain->shouldRecreate() || m_Window->isResized()) {
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
			.setOffset({ 0,0 })
			.setExtent(swapChainDetails.extent);

		vk::ClearColorValue clearColor = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
		vk::ClearValue clearValue = vk::ClearValue(clearColor);
		vk::ClearValue depthStencil = vk::ClearValue().setDepthStencil({ 1.0f ,0 });

		std::array<vk::ClearValue, 2> clearValues = { clearValue, depthStencil };

		vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(m_RenderPass)
			.setFramebuffer(m_VulkanSwapChain->getFrameBuffers()[m_CurrentImageIndex])
			.setRenderArea(renderArea)
			.setClearValues(clearValues);

		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

		m_VulkanPipeline->bind(commandBuffer);
	}

	void VulkanRenderer::endRecoding(vk::CommandBuffer commandBuffer) {
		commandBuffer.endRenderPass();
		commandBuffer.end();
	}
}

