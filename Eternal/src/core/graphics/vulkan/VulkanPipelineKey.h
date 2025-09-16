#pragma once
#include <vulkan/vulkan.hpp>

#include "utils/Base.h"

// namespace Eternal {
//     struct PipelineKey {
//         bool hasMaterial = false;
//         vk::PipelineLayout pipelineLayout = nullptr;
//         vk::RenderPass renderPass = nullptr;
//         bool operator==(const PipelineKey& other) const = default;
//     };
// }
//
// template<>
// struct std::hash<Eternal::PipelineKey> {
//     hash() = default;
//
//     std::size_t operator()(const Eternal::PipelineKey& key) const noexcept {
//         std::size_t seed = 0;
//         Eternal::hashCombine(seed, key.hasMaterial);
//         Eternal::hashCombine(seed, static_cast<VkPipelineLayout>(key.pipelineLayout));
//         Eternal::hashCombine(seed, static_cast<VkRenderPass>(key.renderPass));
//         return seed;
//     }
// };
