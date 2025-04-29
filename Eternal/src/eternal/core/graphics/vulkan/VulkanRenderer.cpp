#include "VulkanRenderer.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include <eternal/core/scene/Entity.h>
#include <eternal/core/scene/TransformComponent.h>

namespace Eternal {

	VulkanRenderer::VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene) : m_Scene(scene), m_Platform(platform)
	{
		ETERNAL_ASSERT(m_Scene != nullptr, "Scene is null");
		ETERNAL_ASSERT(m_Platform != nullptr, "Platform is null");

		m_Camera = Memory::Allocate<Camera>();
		float aspectRatio = (float)window->getWidth() / (float)window->getHeight();
		m_Camera->setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 10.f);

		auto swapchain = m_Platform->createSwapChain(window);
		m_SwapChain = dynamic_cast<VulkanSwapChain*>(swapchain);

		if (!m_SwapChain)
		{
			Eternal::Logger::Error("Failed to create Vulkan SwapChain");
			return;
		}

		m_RenderPass = m_SwapChain->getRenderPass();

		vk::PushConstantRange pushConstantRange = vk::PushConstantRange()
			.setOffset(0)
			.setSize(sizeof(PushConstants))
			.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

		m_PipelineLayout = m_Platform->createPipelineLayout(pushConstantRange);

		m_Pipeline = m_Platform->createPipeline(m_PipelineLayout, m_SwapChain->getRenderPass());

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

		m_Platform->getVkInstance().destroy();

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

	VulkanRenderer::FrameInfo* VulkanRenderer::beginFrame()
	{
		uint32_t imageIndex = 0;

		vk::Result result;

		result = m_SwapChain->acquire(m_ImageAvailableSemaphores[m_CurrentFrame], &imageIndex);

		result = m_LogicalDevice.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT16_MAX);

		result = m_LogicalDevice.resetFences(1, &m_InFlightFences[m_CurrentFrame]);

		vk::CommandBuffer commandBuffer = m_CommandBuffers[m_CurrentFrame];

		commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

		return Memory::Allocate<VulkanFrameInfo>(commandBuffer, imageIndex);
	}

	void VulkanRenderer::render(FrameInfo* frameInfo)
	{
		auto* vkFrameInfo = static_cast<VulkanFrameInfo*>(frameInfo);

		recordCommandBuffer(*vkFrameInfo);

		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&m_ImageAvailableSemaphores[m_CurrentFrame])
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(1)
			.setPCommandBuffers(&(vkFrameInfo->commandBuffer))
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&m_RenderFinishedSemaphores[m_CurrentFrame]);

		auto graphicsQueue = m_Platform->getGraphicsQueue();

		vk::Result result = graphicsQueue.submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);

		m_SwapChain->present(m_RenderFinishedSemaphores[m_CurrentFrame], vkFrameInfo->imageIndex);
	}

	void VulkanRenderer::endFrame()
	{
		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::recordCommandBuffer(VulkanFrameInfo& vkFrameInfo)
	{
		recordCommandBuffer(vkFrameInfo.commandBuffer, vkFrameInfo.imageIndex);
	}

	void VulkanRenderer::recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VulkanSwapChain::SwapChainDetails swapChainDetails = m_SwapChain->getSwapChainDetails();

		vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo();

		commandBuffer.begin(commandBufferBeginInfo);

		vk::Rect2D renderArea = vk::Rect2D()
			.setOffset({ 0,0 })
			.setExtent(swapChainDetails.extent);

		vk::ClearColorValue clearColor = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
		vk::ClearValue clearValue = vk::ClearValue(clearColor);

		vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(m_RenderPass)
			.setFramebuffer(m_SwapChain->getFrameBuffers()[imageIndex])
			.setRenderArea(renderArea)
			.setClearValueCount(1)
			.setPClearValues(&clearValue);

		commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

		for (auto& e : m_Scene->getAllEntityWith<Eternal::TransformComponent>())
		{
			Eternal::Entity entity = Eternal::Entity(e, m_Scene);
			auto& component = entity.getComponent<Eternal::TransformComponent>();

			glm::mat4 modelMatrix = m_Camera->getProjection() * component.mat4();

			m_PushConstants.transform = modelMatrix;

			m_PushConstants.normalMatrix = component.mat4();

			commandBuffer.pushConstants(m_PipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstants), &m_PushConstants);
		}

		m_VulkanBufferManager->bindBuffers(commandBuffer);

		vk::Viewport viewport = vk::Viewport()
			.setX(0.0f)
			.setY(0.0f)
			.setWidth((float)swapChainDetails.extent.width)
			.setHeight((float)swapChainDetails.extent.height)
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f);

		commandBuffer.setViewport(0, 1, &viewport);

		vk::Rect2D scissor = vk::Rect2D()
			.setOffset({ 0,0 })
			.setExtent(swapChainDetails.extent);

		commandBuffer.setScissor(0, 1, &scissor);

		m_VulkanBufferManager->draw(commandBuffer);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		commandBuffer.endRenderPass();

		commandBuffer.end();
	}
}

