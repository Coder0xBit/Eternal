#pragma once
#include "VulkanDescriptorPool.h"
#include "VulkanPlatform.h"

namespace Eternal {
    struct PipelineLayoutCacheKey {
        uint32_t pipelineLayoutMask = 0;
        bool operator==(const PipelineLayoutCacheKey& other) const = default;
    };

    struct PipelineLayoutCacheKeyHasher {
        std::size_t operator()(const Eternal::PipelineLayoutCacheKey& key) const noexcept {
            std::size_t seed = 0;
            Eternal::hashCombine(seed, key.pipelineLayoutMask);
            return seed;
        }
    };

    class VulkanPipelineLayoutCache {
    public:
        using PipelineLayoutContainer = std::unordered_map<PipelineLayoutCacheKey, vk::PipelineLayout,
            PipelineLayoutCacheKeyHasher>;
        VulkanPipelineLayoutCache(VulkanDescriptorPool* descriptorPool, VulkanPlatform* platform);

        ~VulkanPipelineLayoutCache();

        vk::PipelineLayout getOrCreatePipelineLayout(PipelineLayoutCacheKey pipelineLayoutCacheKey);

        VulkanDescriptorSetLayout* getUboDescriptorSetLayout() const { return m_UniformBufferDescriptorSetLayout; }
        VulkanDescriptorSetLayout* getMaterialDescriptorSetLayout() const { return m_MaterialDescriptorSetLayout; }

    private :
        VulkanPlatform* m_Platform = nullptr;
        vk::Device m_LogicalDevice;
        VulkanDescriptorPool* m_DescriptorPool = nullptr;
        PipelineLayoutContainer m_PipelineLayoutCache;
        VulkanDescriptorSetLayout* m_UniformBufferDescriptorSetLayout = nullptr;
        VulkanDescriptorSetLayout* m_MaterialDescriptorSetLayout = nullptr;
    };
}
