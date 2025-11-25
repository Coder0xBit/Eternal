#pragma once

#include "utils/Base.h"
#include "core/graphics/Renderer.h"
#include "core/graphics/vulkan/VulkanBufferManager.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/vulkan/VulkanDescriptorPool.h"
#include "core/graphics/vulkan/VulkanTextureManager.h"
#include "core/graphics/vulkan/VulkanPipelineCache.h"
#include "core/graphics/vulkan/VulkanPipelineLayoutCache.h"
#include "core/scene/Scene.h"

namespace Eternal {
    class ImGuiOverlay;

    constexpr uint32_t e_MaxEntities = 100;

    class VulkanRenderer : public Renderer {
    public:
        struct PushConstants {
            glm::mat4 transform{1.f};
            glm::mat4 normalMatrix{1.f};
            glm::mat4 modelMatrix{1.0f};
        };

        VulkanRenderer(const Builder& builder);

        VulkanRenderer(VulkanPlatform* platform, Window* window, Scene* scene);

        ~VulkanRenderer() override;

        VulkanPlatform* getPlatform() const { return m_Platform; }

        SwapChain* getSwapChain() const override { return m_VulkanSwapChain; }

        bool beginFrame() override;

        void render(Eternal::Camera* camera) override;

        void endFrame() override;

    private:
        void initialize();

        void bindScene();

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
        vk::RenderPass m_RenderPass = nullptr;

        std::vector<std::shared_ptr<VulkanBuffer> > m_UniformBuffers;
        VulkanDescriptorPool* m_DescriptorPool = nullptr;
        std::unordered_map<uint32_t, vk::DescriptorSet> m_UniformDescriptorSets;
        std::unordered_map<uint32_t, vk::DescriptorSet> m_MaterialDescriptorSets;

        Eternal::VulkanPipelineCache* m_PipelineCache = nullptr;
        Eternal::VulkanPipelineLayoutCache* m_PipelineLayoutCache = nullptr;
        vk::Pipeline* m_BoundPipeline = nullptr;
        Eternal::Timer* m_Timer = nullptr;
    };
}
