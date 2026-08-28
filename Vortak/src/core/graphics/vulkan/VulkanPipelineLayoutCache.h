#pragma once

#include "core/graphics/vulkan/VulkanDescriptorPool.h"
#include "core/graphics/vulkan/VulkanDevice.h"

namespace Vortak {
    struct PipelineLayoutCacheKey {
        uint32_t pipelineLayoutMask = 0;
        bool operator==(const PipelineLayoutCacheKey& other) const = default;
    };

    struct PipelineLayoutCacheKeyHasher {
        std::size_t operator()(const Vortak::PipelineLayoutCacheKey& key) const noexcept {
            std::size_t seed = 0;
            Vortak::hashCombine(seed, key.pipelineLayoutMask);
            return seed;
        }
    };

    class VulkanPipelineLayoutCache {
    public:
        using PipelineLayoutContainer = std::unordered_map<PipelineLayoutCacheKey, vk::PipelineLayout,
            PipelineLayoutCacheKeyHasher>;
        VulkanPipelineLayoutCache(VulkanDescriptorPool* descriptorPool, VulkanDevice* platform);

        ~VulkanPipelineLayoutCache();

        vk::PipelineLayout getOrCreatePipelineLayout(PipelineLayoutCacheKey pipelineLayoutCacheKey);

        VulkanDescriptorSetLayout* getUboDescriptorSetLayout() const { return mUniformBufferDescriptorSetLayout; }
        VulkanDescriptorSetLayout* getMaterialDescriptorSetLayout() const { return mMaterialDescriptorSetLayout; }

    private :
        VulkanDevice* mPlatform = nullptr;
        vk::Device mLogicalDevice;
        VulkanDescriptorPool* mDescriptorPool = nullptr;
        PipelineLayoutContainer mPipelineLayoutCache;
        VulkanDescriptorSetLayout* mUniformBufferDescriptorSetLayout = nullptr;
        VulkanDescriptorSetLayout* mMaterialDescriptorSetLayout = nullptr;
    };
}
