#pragma once
#include "VulkanPlatform.h"
#include "glm/fwd.hpp"

namespace Eternal {
    enum class DescriptorSetBindingPoints : uint8_t {
        UBO = 0,
        MATERIAL = 1 << 0,
    };

    struct PipelineLayoutKey {
        uint8_t layoutMask = 0;
    };

    struct PipelineLayoutKeyHasher {
        std::size_t operator()(const Eternal::PipelineLayoutKey& key) const noexcept {
            std::size_t seed = 0;
            Eternal::hashCombine(seed, key.layoutMask);
            return seed;
        }
    };


    class VulkanPipelineLayoutCache {
    public :
        VulkanPipelineLayoutCache(VulkanPlatform* vulkanPlatform) : m_Platform(vulkanPlatform) {
        }

        vk::PipelineLayout getOrCreate(PipelineLayoutKey layoutKey);

    private:
        VulkanPlatform* m_Platform = nullptr;
        std::unordered_map<PipelineLayoutKey, vk::PipelineLayout, PipelineLayoutKeyHasher> m_LayoutCache;
    };
}
