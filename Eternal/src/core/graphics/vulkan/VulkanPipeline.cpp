#include "VulkanPipeline.h"

namespace Eternal {
	VulkanPipeline::VulkanPipeline(vk::Device logicalDevice)
		: m_LogicalDevice(logicalDevice) {
	}

	void VulkanPipeline::bindLayout(vk::PipelineLayout pipelineLayout) {
		m_CreationRequirements.pipelineLayout = pipelineLayout;
	}

	void VulkanPipeline::bindRenderPass(vk::RenderPass renderPass) {
		m_CreationRequirements.renderPass = renderPass;
	}

	void VulkanPipeline::bindVertexShader(vk::ShaderModule vertexShader) {
		m_CreationRequirements.vertexShader = vertexShader;
	}

	void VulkanPipeline::bindFragmentShader(vk::ShaderModule fragmentShader) {
		m_CreationRequirements.fragmentShader = fragmentShader;
	}

	void VulkanPipeline::bindVertexBindingDescriptions(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions) {
		m_CreationRequirements.vertexInputBindingDescriptions = bindingDescriptions;
	}

	void VulkanPipeline::bindVertexAttributeDescriptions(const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions) {
		m_CreationRequirements.vertexInputAttributeDescriptions = attributeDescriptions;
	}

	void VulkanPipeline::create() {
		create(m_CreationRequirements);
	}

	void VulkanPipeline::create(const PipelineCreationRequirements& pipelineCreationRequirements) {
		m_CreationRequirements = pipelineCreationRequirements;

		vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(m_CreationRequirements.vertexShader)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(m_CreationRequirements.fragmentShader)
			.setPName("main");

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
			vertexShaderStageCreateInfo,
			fragmentShaderStageCreateInfo
		};

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(VK_FALSE);

		vk::PipelineViewportStateCreateInfo viewPortStateCreateInfo = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setScissorCount(1);

		vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(VK_TRUE);

		vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(VK_FALSE)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		vk::PipelineDepthStencilStateCreateInfo depthStencil = vk::PipelineDepthStencilStateCreateInfo()
			.setDepthTestEnable(VK_TRUE)
			.setDepthWriteEnable(VK_TRUE)
			.setDepthCompareOp(vk::CompareOp::eLess)
			.setDepthBoundsTestEnable(VK_FALSE)
			.setStencilTestEnable(VK_FALSE);

		vk::PipelineColorBlendAttachmentState colorBlendAttachmentstate = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(VK_FALSE);

		vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(VK_FALSE)
			.setLogicOp(vk::LogicOp::eCopy)
			.setAttachmentCount(1)
			.setPAttachments(&colorBlendAttachmentstate)
			.setBlendConstants({ 0.0f,0.0f,0.0f,0.0f });

		std::vector<vk::DynamicState> dynamicState = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
			.setDynamicStates(dynamicState);

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptions(m_CreationRequirements.vertexInputBindingDescriptions)
			.setVertexAttributeDescriptions(m_CreationRequirements.vertexInputAttributeDescriptions);

		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
			.setStages(shaderStages)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssemblyInfo)
			.setPViewportState(&viewPortStateCreateInfo)
			.setPDepthStencilState(&depthStencil)
			.setPRasterizationState(&rasterizationStateCreateInfo)
			.setPMultisampleState(&multiSampleStateCreateInfo)
			.setPColorBlendState(&colorBlendStateCreateInfo)
			.setPDynamicState(&dynamicStateCreateInfo)
			.setLayout(m_CreationRequirements.pipelineLayout)
			.setRenderPass(m_CreationRequirements.renderPass)
			.setSubpass(0);

		try {
			auto [result, graphicsPipeline] = m_LogicalDevice.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo, nullptr);
			m_Pipeline = graphicsPipeline;
		}
		catch (vk::SystemError err) {
			Eternal::Logger::Error("Graphics Pipeline Exception : {}", err.what());
		};
	}

	void VulkanPipeline::bind(vk::CommandBuffer commandBuffer) {
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
	}

	VulkanPipeline::~VulkanPipeline() {
		m_LogicalDevice.destroyPipeline(m_Pipeline);
	}
}