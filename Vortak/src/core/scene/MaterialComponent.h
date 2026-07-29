#pragma once

#include "utils/Base.h"
#include "core/resource/Image.h"

namespace Vortak {
    enum PipelineParams {
        NONE = BIT(0),
        UBO = BIT(1),
        SAMPLER = BIT(3),
    };

    constexpr uint32_t eDefaultPipelineLayoutBitMask = PipelineParams::UBO;

    struct MaterialComponent {
        Image* albedoTexture = nullptr;
        uint32_t pipelineLayoutBitMask = PipelineParams::UBO | PipelineParams::SAMPLER;
    };
}
