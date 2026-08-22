#pragma once
#include "core/graphics/Shader.h"
#include "core/graphics/ShaderType.h"

#include <vulkan/vulkan.hpp>

namespace Vortak {
    class VulkanShader : public Shader {
    public:
        VulkanShader(GraphicsPlatform* graphicsPlatform, ShaderProgram* shaderProgram, ShaderType shaderType);

        ~VulkanShader() override;

        void compile() override;

        vk::ShaderModule getShaderModule() const { return mShaderModule; }

    private :
        vk::ShaderModule mShaderModule = nullptr;
        vk::Device mLogicalDevice = nullptr;
    };
}
