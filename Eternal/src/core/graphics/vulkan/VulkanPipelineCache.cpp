#include "VulkanPipelineCache.h"

#include "VulkanPipeline.h"

namespace Eternal {
    VulkanPipelineCache::~VulkanPipelineCache() {
        vk::Device logicalDevice = m_Platform->getLogicalDevice();
        for (auto& [pipelineKey, pipeline]: m_PipelineCache) {
            logicalDevice.destroyPipeline(pipeline->getPipeline());
        }
    }

    vk::Pipeline VulkanPipelineCache::getOrCreate(PipelineKey pipelineKey) {
        vk::Device logicalDevice = m_Platform->getLogicalDevice();
        std::string fragShader;
        if (pipelineKey.hasMaterial) {
            fragShader = "frag_texture_mapping.spv";
        } else {
            fragShader = "common_fragment.spv";
        }

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
        return m_PipelineCache.emplace(pipelineKey, vulkanPipeline).first->second->getPipeline();
    }
}
