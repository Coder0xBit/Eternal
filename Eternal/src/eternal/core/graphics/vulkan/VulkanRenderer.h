#pragma once 

#include <eternal/core/graphics/Renderer.h>
#include <eternal/core/ecs/EntityManager.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/utils/Base.h>

namespace Eternal {



	class VulkanRenderer : public Renderer {

	public:
		struct PushConstants {
			glm::mat4 transform{ 1.f };
		};

		struct VulkanFrameInfo : public Renderer::FrameInfo {
			VulkanFrameInfo(vk::CommandBuffer commandBuffer, uint32_t imageIndex) :
				commandBuffer(commandBuffer), imageIndex(imageIndex) {
			}

			bool operator==(const VulkanFrameInfo& other) noexcept {
				return this->commandBuffer == other.commandBuffer && this->imageIndex == other.imageIndex;
			}

			vk::CommandBuffer commandBuffer;
			uint32_t imageIndex;
		};

		VulkanRenderer(Memory::Ref<VulkanPlatform> platform, Memory::Ref<Window> window, Memory::Ref<EntityManager> entityManager);

		~VulkanRenderer();

		FrameInfo beginFrame() override;

		void render(FrameInfo frameInfo) override;

		void endFrame() override;

	private:

		void createCommandPool();

		void createCommandBuffers();

		void createSemaphores();

		void createFences();

		void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

		void recordCommandBuffer(VulkanFrameInfo& vkFrameInfo);

		Memory::Ref<EntityManager> m_EntityManager;
		Memory::Ref<VulkanPlatform> m_Platform;
		Memory::Ref<VulkanSwapChain> m_SwapChain;
		Memory::Ref<Eternal::VulkanBufferManager> m_VulkanBufferManager;

		Memory::Ref<Eternal::Camera> m_Camera;

		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
		std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
		std::vector<vk::Fence> m_InFlightFences;

		uint32_t m_CurrentFrame = 0;

		vk::CommandPool m_CommandPool = nullptr;
		std::vector<vk::CommandBuffer> m_CommandBuffers;

		vk::Device m_LogicalDevice = nullptr;
		vk::PhysicalDevice m_PhysicalDevice = nullptr;
		PushConstants m_PushConstants;

		vk::PipelineLayout m_PipelineLayout = nullptr;
		vk::Pipeline m_Pipeline = nullptr;
		vk::RenderPass m_RenderPass = nullptr;
	};
}