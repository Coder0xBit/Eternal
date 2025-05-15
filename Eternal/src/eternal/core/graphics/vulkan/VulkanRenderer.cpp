#include "VulkanRenderer.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include <eternal/core/scene/Entity.h>
#include <eternal/core/scene/TransformComponent.h>

namespace Eternal {

	VulkanRenderer::VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene) : m_Scene(scene), m_Platform(platform), m_Window(window)
	{
		ETERNAL_ASSERT(m_Scene != nullptr, "Scene is null");
		ETERNAL_ASSERT(m_Platform != nullptr, "Platform is null");
		ETERNAL_ASSERT(m_Window != nullptr, "Window is null");

		m_Camera = Memory::Allocate<Camera>();
		float aspectRatio = m_Window->getAspectRatio();
		m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

		auto swapchain = m_Platform->createSwapChain(window);
		m_SwapChain = dynamic_cast<VulkanSwapChain*>(swapchain);

		if (!m_SwapChain)
		{
			Eternal::Logger::Error("Failed to create Vulkan SwapChain");
			return;
		}

		m_RenderPass = m_SwapChain->getRenderPass();
		ETERNAL_ASSERT(m_RenderPass, "Render pass is null");

		vk::PushConstantRange pushConstantRange = vk::PushConstantRange()
			.setOffset(0)
			.setSize(sizeof(PushConstants))
			.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

		m_PipelineLayout = m_Platform->createPipelineLayout(pushConstantRange);

		m_Pipeline = m_Platform->createPipeline(m_PipelineLayout, m_RenderPass);

		m_LogicalDevice = m_Platform->getLogicalDevice();

		m_PhysicalDevice = m_Platform->getPhysicalDevice();

		createCommandPool();

		createCommandBuffers();

		createSemaphores();

		createFences();

		m_VulkanBufferManager = Memory::Allocate<VulkanBufferManager>(m_LogicalDevice, m_PhysicalDevice, m_Scene);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		m_LogicalDevice.waitIdle();

		for (auto semaphore : m_ImageAvailableSemaphores)
		{
			m_LogicalDevice.destroySemaphore(semaphore);
		}

		for (auto semaphore : m_RenderFinishedSemaphores)
		{
			m_LogicalDevice.destroySemaphore(semaphore);
		}

		for (auto fence : m_InFlightFences)
		{
			m_LogicalDevice.destroyFence(fence);
		}

		m_LogicalDevice.destroyCommandPool(m_CommandPool);

		m_LogicalDevice.destroyPipeline(m_Pipeline);

		m_LogicalDevice.destroyPipelineLayout(m_PipelineLayout);

		Memory::Deallocate(m_SwapChain);

		Memory::Deallocate(m_VulkanBufferManager);

		m_LogicalDevice.destroy();

		Memory::Deallocate(m_Platform);

		Memory::Deallocate(m_Camera);
	}

	void VulkanRenderer::createCommandPool()
	{
		auto graphicsQueueIndex = m_Platform->getGraphicsQueueIndex();

		vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(graphicsQueueIndex);

		m_CommandPool = m_LogicalDevice.createCommandPool(commandPoolCreateInfo);
	}

	void VulkanRenderer::createCommandBuffers()
	{
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(m_CommandPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

		m_CommandBuffers = m_LogicalDevice.allocateCommandBuffers(commandBufferAllocateInfo);
	}

	void VulkanRenderer::createSemaphores()
	{
		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();

			m_ImageAvailableSemaphores[i] = m_LogicalDevice.createSemaphore(semaphoreCreateInfo);
			m_RenderFinishedSemaphores[i] = m_LogicalDevice.createSemaphore(semaphoreCreateInfo);
		}
	}

	void VulkanRenderer::createFences()
	{
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::FenceCreateInfo fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

			m_InFlightFences[i] = m_LogicalDevice.createFence(fenceCreateInfo);
		}
	}

	void VulkanRenderer::handleWindowResize()
	{
		Eternal::Logger::Debug("Window resized to {}x{}", m_Window->getWidth(), m_Window->getHeight());

		if (m_Window->isMinimized())
			return;

		m_Window->setWindowResized(false);
		m_SwapChain->setShouldRecreate(false);

		m_LogicalDevice.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT16_MAX);
		m_LogicalDevice.resetFences(1, &m_InFlightFences[m_CurrentFrame]);

		m_LogicalDevice.waitIdle();

		m_SwapChain->recreate();
		m_RenderPass = m_SwapChain->getRenderPass();

		float aspectRatio = m_Window->getAspectRatio();
		m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);
	}

	FrameInfo* VulkanRenderer::beginFrame()
	{
		if (m_Window->isMinimized())
			return nullptr;

		vk::Result result;

		m_SwapChain->acquire(m_ImageAvailableSemaphores[m_CurrentFrame], &m_CurrentImageIndex);

		if (m_SwapChain->shouldRecreate() || m_Window->isResized())
		{
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

	void VulkanRenderer::render()
	{
		VulkanSwapChain::SwapChainDetails swapChainDetails = m_SwapChain->getSwapChainDetails();

		for (auto& e : m_Scene->getAllEntityWith<Eternal::TransformComponent>())
		{
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);
			auto& component = entity.getComponent<Eternal::TransformComponent>();

			glm::mat4 modelMatrix = m_Camera->getProjection() * component.mat4();

			m_PushConstants.transform = modelMatrix;

			m_PushConstants.normalMatrix = component.mat4();

			m_CurrentCommandBuffer.pushConstants(m_PipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstants), &m_PushConstants);
		}

		m_VulkanBufferManager->bindBuffers(m_CurrentCommandBuffer);

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

		m_VulkanBufferManager->draw(m_CurrentCommandBuffer);
	}

	void VulkanRenderer::endFrame()
	{
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

		m_SwapChain->present(m_RenderFinishedSemaphores[m_CurrentFrame], m_CurrentImageIndex);

		if (m_SwapChain->shouldRecreate() || m_Window->isResized())
		{
			handleWindowResize();
			return;
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::beginRecording(vk::CommandBuffer commandBuffer)
	{
		VulkanSwapChain::SwapChainDetails swapChainDetails = m_SwapChain->getSwapChainDetails();

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
			.setFramebuffer(m_SwapChain->getFrameBuffers()[m_CurrentImageIndex])
			.setRenderArea(renderArea)
			.setClearValues(clearValues);

		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
	}

	void VulkanRenderer::endRecoding(vk::CommandBuffer commandBuffer)
	{
		commandBuffer.endRenderPass();

		commandBuffer.end();
	}
}

