#pragma once 

#include <eternal/core/graphics/Renderer.h>
#include <eternal/core/scene/Scene.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/graphics/vulkan/VulkanFrameInfo.h>
#include <eternal/utils/Base.h>
#include <eternal/core/graphics/vulkan/VulkanUtils.h>

namespace Eternal {

	class VulkanRenderer : public Renderer {

	public:
		struct PushConstants {
			glm::mat4 transform{ 1.f };
			glm::mat4 normalMatrix{ 1.f };
		};

		struct UniformBuffer {
			glm::mat4 transform{ 1.f };
			glm::mat4 normalMatrix{ 1.f };
		};

		VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene);

		~VulkanRenderer();

		VulkanPlatform* getPlatform() { return m_Platform; }

		VulkanSwapChain* getSwapChain() { return m_VulkanSwapChain; }

		FrameInfo* beginFrame() override;

		void render() override;

		void endFrame() override;

	private:

		void createUniformBuffers();

		void createCommandPool();

		void createCommandBuffers();

		void createSemaphores();

		void createFences();

		void handleWindowResize();

		void beginRecording(vk::CommandBuffer commandBuffer);

		void endRecoding(vk::CommandBuffer commandBuffer);

		Scene* m_Scene;

		VulkanPlatform* m_Platform;

		VulkanSwapChain* m_VulkanSwapChain;

		Eternal::VulkanBufferManager* m_VulkanBufferManager;

		Eternal::Camera* m_Camera;

		Eternal::Window* m_Window;

		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;

		std::vector<vk::Semaphore> m_RenderFinishedSemaphores;

		std::vector<vk::Fence> m_InFlightFences;

		uint32_t m_CurrentFrame = 0;

		vk::CommandPool m_CommandPool = nullptr;

		std::vector<vk::CommandBuffer> m_CommandBuffers;

		vk::CommandBuffer m_CurrentCommandBuffer = nullptr;

		uint32_t m_CurrentImageIndex = 0;

		vk::Device m_LogicalDevice = nullptr;

		vk::PhysicalDevice m_PhysicalDevice = nullptr;

		PushConstants m_PushConstants;

		vk::PipelineLayout m_PipelineLayout = nullptr;

		vk::Pipeline m_Pipeline = nullptr;

		vk::RenderPass m_RenderPass = nullptr;

		std::vector<std::shared_ptr<VulkanBuffer>> m_UniformBuffers;
	};
}