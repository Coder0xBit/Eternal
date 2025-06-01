#pragma once

#include <vulkan/vulkan.hpp>

#include <utils/Base.h>
#include <core/graphics/Vertex.h>

namespace Eternal {

	class VulkanPipeline {
	public:
		static constexpr uint32_t const SHADER_MODULE_COUNT = 2;

		struct PipelineCreationRequirements {
			vk::PipelineCreateFlags flags = vk::PipelineCreateFlags();
			vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics;
			vk::PipelineLayout pipelineLayout = nullptr;
			vk::RenderPass renderPass = nullptr;
			vk::ShaderModule vertexShader = nullptr;
			vk::ShaderModule fragmentShader = nullptr;
			std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions = Eternal::Vertex::getBindingDescription();
			std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = Eternal::Vertex::getAttributeDescription();
		};

		VulkanPipeline(vk::Device logicalDevice);
		void bindLayout(vk::PipelineLayout pipelineLayout);
		void bindRenderPass(vk::RenderPass renderPass);
		void bindVertexShader(vk::ShaderModule vertexShader);
		void bindFragmentShader(vk::ShaderModule fragmentShader);
		void bindVertexBindingDescriptions(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions);
		void bindVertexAttributeDescriptions(const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions);
		void create();
		void create(const PipelineCreationRequirements& pipelineCreationRequirements);
		void bind(vk::CommandBuffer commandBuffer);
		vk::Pipeline getPipeline() const { return m_Pipeline; }
		~VulkanPipeline();

	private:
		vk::Device m_LogicalDevice = nullptr;
		vk::Pipeline m_Pipeline = nullptr;
		PipelineCreationRequirements m_CreationRequirements;
	};

}