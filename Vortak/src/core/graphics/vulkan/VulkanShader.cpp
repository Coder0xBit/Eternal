#include "core/graphics/vulkan/VulkanShader.h"
#include "core/graphics/vulkan/VulkanDevice.h"

namespace Vortak {
    VulkanShader::VulkanShader(GraphicsDevice* graphicsDevice, ShaderProgram* shaderProgram, ShaderType shaderType)
        : Shader(graphicsDevice, shaderProgram, shaderType) {
        auto* vulkanPlatform = dynamic_cast<VulkanDevice*>(graphicsDevice);
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
