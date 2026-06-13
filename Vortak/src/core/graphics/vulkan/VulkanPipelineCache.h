#pragma once

#include "core/graphics/vulkan/VulkanPipeline.h"
#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/scene/MaterialComponent.h"

#include <vulkan/vulkan.hpp>

namespace Vortak {
    struct PipelineKey {
        uint32_t pipelineLayoutMask = 0;
        vk::PipelineLayout pipelineLayout = nullptr;
        vk::RenderPass renderPass = nullptr;
        MaterialComponent* material = nullptr;
        bool operator==(const PipelineKey& other) const = default;
    };

    struct PipelineKeyHasher {
        std::size_t operator()(const Vortak::PipelineKey& key) const noexcept {
            std::size_t seed = 0;
            Vortak::hashCombine(seed, key.pipelineLayoutMask);
            Vortak::hashCombine(seed, key.material);
            Vortak::hashCombine(seed, static_cast<VkPipelineLayout>(key.pipelineLayout));
            Vortak::hashCombine(seed, static_cast<VkRenderPass>(key.renderPass));
            return seed;
        }
    };


    class VulkanPipelineCache {
    public :
        using PipelineContainer = std::unordered_map<PipelineKey, std::shared_ptr<VulkanPipeline>, PipelineKeyHasher>;

        VulkanPipelineCache(VulkanPlatform* platform);
        ~VulkanPipelineCache();

        vk::Pipeline getOrCreate(PipelineKey pipelineKey);

    private :
        VulkanPlatform* mPlatform = nullptr;
        PipelineContainer mPipelineCache;
    };
}