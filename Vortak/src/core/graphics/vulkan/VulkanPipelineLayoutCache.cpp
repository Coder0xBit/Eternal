#include "core/graphics/vulkan/VulkanPipelineLayoutCache.h"
#include "core/scene/MaterialComponent.h"

#include <ranges>

namespace Vortak {
    VulkanPipelineLayoutCache::VulkanPipelineLayoutCache(VulkanDescriptorPool* descriptorPool, VulkanDevice* platform)
        : mPlatform(platform), mDescriptorPool(descriptorPool) {
        mLogicalDevice = mPlatform->getLogicalDevice();
        mUniformBufferDescriptorSetLayout = VulkanDescriptorSetLayout::Builder(mLogicalDevice)
                .addBinding({0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex})
                .build();

        mMaterialDescriptorSetLayout = VulkanDescriptorSetLayout::Builder(mLogicalDevice)
                .addBinding({0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment})
                .build();
    }

    VulkanPipelineLayoutCache::~VulkanPipelineLayoutCache() {
        mLogicalDevice.destroyDescriptorSetLayout(mUniformBufferDescriptorSetLayout->getDescriptorSetLayout());
        mLogicalDevice.destroyDescriptorSetLayout(mMaterialDescriptorSetLayout->getDescriptorSetLayout());
        for (auto& pipelineLayout: mPipelineLayoutCache | std::views::values) {
            mLogicalDevice.destroyPipelineLayout(pipelineLayout);
        }
    }

    vk::PipelineLayout VulkanPipelineLayoutCache::getOrCreatePipelineLayout(
        PipelineLayoutCacheKey pipelineLayoutCacheKey) {
        if (mPipelineLayoutCache.contains(pipelineLayoutCacheKey)) {
            return mPipelineLayoutCache[pipelineLayoutCacheKey];
        }

        std::vector<vk::DescriptorSetLayout> setLayouts;
        if (IS_BIT_SET(pipelineLayoutCacheKey.pipelineLayoutMask, PipelineParams::UBO)) {
            setLayouts.emplace_back(mUniformBufferDescriptorSetLayout->getDescriptorSetLayout());
        }

        if (IS_BIT_SET(pipelineLayoutCacheKey.pipelineLayoutMask, PipelineParams::SAMPLER)) {
            setLayouts.emplace_back(mMaterialDescriptorSetLayout->getDescriptorSetLayout());
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
                .setSetLayouts(setLayouts);

        vk::PipelineLayout pipelineLayout = mLogicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
        mPipelineLayoutCache.emplace(pipelineLayoutCacheKey, pipelineLayout);
        return pipelineLayout;
    }
}
