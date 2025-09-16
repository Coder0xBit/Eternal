#include "VulkanPipelineLayoutCache.h"

#include "VulkanPipelineCache.h"

namespace Eternal {
    vk::PipelineLayout VulkanPipelineLayoutCache::getOrCreate(PipelineLayoutKey layoutKey) {
        return vk::PipelineLayout();
    }
}
