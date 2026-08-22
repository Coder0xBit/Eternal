#include "core/graphics/vulkan/VulkanShader.h"
#include "core/graphics/vulkan/VulkanPlatform.h"

namespace Vortak {
    VulkanShader::VulkanShader(GraphicsPlatform* graphicsPlatform, ShaderProgram* shaderProgram, ShaderType shaderType)
        : Shader(graphicsPlatform, shaderProgram, shaderType) {
        auto* vulkanPlatform = dynamic_cast<VulkanPlatform*>(graphicsPlatform);
        mLogicalDevice = vulkanPlatform->getLogicalDevice();

        const uint32_t* shaderCode = reinterpret_cast<uint32_t*>(shaderProgram->blob.data());

        vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
                                                           .setCodeSize(shaderProgram->blob.size())
                                                           .setPCode(shaderCode);

        mShaderModule = mLogicalDevice.createShaderModule(shaderModuleCreateInfo);
    }

    void VulkanShader::compile() {}

    VulkanShader::~VulkanShader() {
        mLogicalDevice.destroyShaderModule(mShaderModule);
    }
}
