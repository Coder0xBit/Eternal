#include "VulkanPipelineLayoutCache.h"

#include <ranges>

#include "core/scene/MaterialComponent.h"

namespace Eternal {
    VulkanPipelineLayoutCache::VulkanPipelineLayoutCache(VulkanDescriptorPool* descriptorPool, VulkanPlatform* platform)
        : m_Platform(platform), m_DescriptorPool(descriptorPool) {
        m_LogicalDevice = m_Platform->getLogicalDevice();
        m_UniformBufferDescriptorSetLayout = VulkanDescriptorSetLayout::Builder(m_LogicalDevice)
                .addBinding({0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex})
                .build();

        m_MaterialDescriptorSetLayout = VulkanDescriptorSetLayout::Builder(m_LogicalDevice)
                .addBinding({0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment})
                .build();
    }

    VulkanPipelineLayoutCache::~VulkanPipelineLayoutCache() {
        m_LogicalDevice.destroyDescriptorSetLayout(m_UniformBufferDescriptorSetLayout->getDescriptorSetLayout());
        m_LogicalDevice.destroyDescriptorSetLayout(m_MaterialDescriptorSetLayout->getDescriptorSetLayout());
        for (auto& pipelineLayout: m_PipelineLayoutCache | std::views::values) {
            m_LogicalDevice.destroyPipelineLayout(pipelineLayout);
        }
    }

    vk::PipelineLayout VulkanPipelineLayoutCache::getOrCreatePipelineLayout(
        PipelineLayoutCacheKey pipelineLayoutCacheKey) {
        if (m_PipelineLayoutCache.contains(pipelineLayoutCacheKey)) {
            return m_PipelineLayoutCache[pipelineLayoutCacheKey];
        }

        std::vector<vk::DescriptorSetLayout> setLayouts;
        if (IS_BIT_SET(pipelineLayoutCacheKey.pipelineLayoutMask, PipelineParams::UBO)) {
            setLayouts.emplace_back(m_UniformBufferDescriptorSetLayout->getDescriptorSetLayout());
        }

        if (IS_BIT_SET(pipelineLayoutCacheKey.pipelineLayoutMask, PipelineParams::SAMPLER)) {
            setLayouts.emplace_back(m_MaterialDescriptorSetLayout->getDescriptorSetLayout());
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
                .setSetLayouts(setLayouts);

        vk::PipelineLayout pipelineLayout = m_LogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
        m_PipelineLayoutCache.emplace(pipelineLayoutCacheKey, pipelineLayout);
        return pipelineLayout;
    }
}
