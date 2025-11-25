#include "core/graphics/vulkan/VulkanPipelineCache.h"
#include "core/graphics/vulkan/VulkanPipeline.h"
#include "core/scene/MaterialComponent.h"

#include <ranges>

namespace Eternal {
    VulkanPipelineCache::VulkanPipelineCache(VulkanPlatform* platform) : m_Platform(platform) {
    }

    VulkanPipelineCache::~VulkanPipelineCache() {
        m_PipelineCache.clear();
    }

    vk::Pipeline VulkanPipelineCache::getOrCreate(PipelineKey pipelineKey) {
        if (m_PipelineCache.contains(pipelineKey)) {
            return m_PipelineCache[pipelineKey]->getPipeline();
        }

        vk::Device logicalDevice = m_Platform->getLogicalDevice();
        bool isSamplerAttached = IS_BIT_SET(pipelineKey.pipelineLayoutMask, PipelineParams::SAMPLER);
        bool hasMaterial = pipelineKey.material != nullptr;
        std::string fragShader = hasMaterial ? "frag_texture_mapping.spv" : "common_fragment.spv";

        std::string vertexShader = "common_vertex.spv";

        vk::ShaderModule vertexShaderModule = m_Platform->loadShader(
            logicalDevice, "res/shader/bin/" + vertexShader);

        vk::ShaderModule fragmentShaderModule = m_Platform->loadShader(
            logicalDevice, "res/shader/bin/" + fragShader);

        vk::PipelineLayout pipelineLayout = pipelineKey.pipelineLayout;
        vk::RenderPass renderPass = pipelineKey.renderPass;

        std::shared_ptr<VulkanPipeline> vulkanPipeline = std::make_shared<VulkanPipeline>(logicalDevice);
        vulkanPipeline->bindLayout(pipelineLayout);
        vulkanPipeline->bindRenderPass(renderPass);
        vulkanPipeline->bindVertexShader(vertexShaderModule);
        vulkanPipeline->bindFragmentShader(fragmentShaderModule);
        vulkanPipeline->create();
        logicalDevice.destroyShaderModule(vertexShaderModule);
        logicalDevice.destroyShaderModule(fragmentShaderModule);
        auto [it , inserted] = m_PipelineCache.emplace(pipelineKey, vulkanPipeline);
        return it->second->getPipeline();
    }
}
