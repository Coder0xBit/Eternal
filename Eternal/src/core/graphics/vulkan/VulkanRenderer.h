#pragma once 

#include <utils/Base.h>

#include <core/graphics/Renderer.h>
#include <core/scene/Scene.h>

#include "VulkanPlatform.h"
#include "VulkanFrameInfo.h"
#include "VulkanUtils.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescsriptorSetLayout.h"
#include "VulkanTextureManager.h"
#include "VulkanPipeline.h"

namespace Eternal {

	constexpr uint32_t e_MaxEntities = 100;

	class VulkanRenderer : public Renderer {

	public:

		struct PushConstants {
			glm::mat4 transform{ 1.f };
			glm::mat4 normalMatrix{ 1.f };
			glm::mat4 modelMatrix{ 1.0f };
		};

		VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene);
		~VulkanRenderer();
		VulkanPlatform* getPlatform() { return m_Platform; }
		VulkanSwapChain* getSwapChain() { return m_VulkanSwapChain; }
		FrameInfo* beginFrame() override;
		void render() override;
		void endFrame() override;

	private:

		void bindScene();
		void createPipeline();
		void initializeDescriptors();
		void createCommandPool();
		void createCommandBuffers();
		void createSemaphores();
		void createFences();
		void handleWindowResize();
		void beginRecording(vk::CommandBuffer commandBuffer);
		void endRecoding(vk::CommandBuffer commandBuffer);
		void updateUniformBuffers();

		Scene* m_Scene = nullptr;
		VulkanPlatform* m_Platform = nullptr;
		VulkanSwapChain* m_VulkanSwapChain = nullptr;

		vk::Viewport m_Viewport;
		vk::Rect2D m_Scissor;

		Eternal::VulkanBufferManager* m_VulkanBufferManager = nullptr;
		Eternal::VulkanTextureManager* m_VulkanTextureManager = nullptr;
		Eternal::Camera* m_Camera = nullptr;
		Eternal::Window* m_Window = nullptr;

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
		vk::RenderPass m_RenderPass = nullptr;

		VulkanPipeline* m_VulkanPipeline = nullptr;

		std::vector<std::shared_ptr<VulkanBuffer>> m_UniformBuffers;
		VulkanDescriptorPool* m_DescriptorPool = nullptr;
		VulkanDescriptorSetLayout* m_DescriptorSetLayout = nullptr;
		std::unordered_map<uint32_t, vk::DescriptorSet> m_DescriptorSets;
	};
}