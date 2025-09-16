#pragma once
#include <vulkan/vulkan.hpp>

#include "VulkanPipeline.h"
#include "VulkanPipelineKey.h"
#include "VulkanPlatform.h"

namespace Eternal {
    struct PipelineKey {
        bool hasMaterial = false;
        vk::PipelineLayout pipelineLayout = nullptr;
        vk::RenderPass renderPass = nullptr;
        bool operator==(const PipelineKey& other) const = default;
    };

    struct PipelineKeyHasher {
        std::size_t operator()(const Eternal::PipelineKey& key) const noexcept {
            std::size_t seed = 0;
            Eternal::hashCombine(seed, key.hasMaterial);
            Eternal::hashCombine(seed, static_cast<VkPipelineLayout>(key.pipelineLayout));
            Eternal::hashCombine(seed, static_cast<VkRenderPass>(key.renderPass));
            return seed;
        }
    };

    class VulkanPipelineCache {
    public :
        VulkanPipelineCache(VulkanPlatform* vulkanPlatform) : m_Platform(vulkanPlatform) {
        }

        ~VulkanPipelineCache();

        vk::Pipeline getOrCreate(PipelineKey pipelineKey);

    private :
        VulkanPlatform* m_Platform = nullptr;

        std::unordered_map<PipelineKey, std::shared_ptr<VulkanPipeline>, PipelineKeyHasher> m_PipelineCache;
    };
}
