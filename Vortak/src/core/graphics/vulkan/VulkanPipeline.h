#pragma once

#include "core/graphics/Vertex.h"

#include <vulkan/vulkan.hpp>

namespace Vortak {
    class VulkanPipeline {
    public:
        struct PipelineCreationRequirements {
            vk::PipelineCreateFlags flags = vk::PipelineCreateFlags();
            vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics;
            vk::PipelineLayout pipelineLayout = nullptr;
            vk::RenderPass renderPass = nullptr;
            vk::ShaderModule vertexShader = nullptr;
            vk::ShaderModule fragmentShader = nullptr;
            std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions =
                    Vortak::Vertex::getBindingDescription();
            std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions =
                    Vortak::Vertex::getAttributeDescription();
        };

        VulkanPipeline(vk::Device logicalDevice);
        void bindLayout(vk::PipelineLayout pipelineLayout);
        void bindRenderPass(vk::RenderPass renderPass);
        void bindVertexShader(vk::ShaderModule vertexShader);
        void bindFragmentShader(vk::ShaderModule fragmentShader);
        void bindVertexBindingDescriptions(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions);
        void bindVertexAttributeDescriptions(
            const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions);
        void create();
        void create(const PipelineCreationRequirements& pipelineCreationRequirements);
        void bind(vk::CommandBuffer commandBuffer) const;
        vk::Pipeline getPipeline() const { return mPipeline; }
        ~VulkanPipeline();

    private:
        vk::Device mLogicalDevice = nullptr;
        vk::Pipeline mPipeline = nullptr;
        PipelineCreationRequirements mCreationRequirements;
    };
}
