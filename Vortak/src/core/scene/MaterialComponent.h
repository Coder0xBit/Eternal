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
    public:
        MaterialComponent() = default;
        ~MaterialComponent() = default;

        MaterialComponent(Image* albedoTexture = nullptr) {
            mAlbedoTexture = albedoTexture;
            if (mAlbedoTexture) {
                mPipelineLayoutBitMask = mPipelineLayoutBitMask |= PipelineParams::SAMPLER;
            }
        }

        uint32_t getPipelineLayoutBitMask() const { return mPipelineLayoutBitMask; }

        const Image* getAlbedoTexture() const { return mAlbedoTexture; }

    private:
        Image* mAlbedoTexture = nullptr;
        uint32_t mPipelineLayoutBitMask = PipelineParams::UBO | PipelineParams::SAMPLER;
    };
}
