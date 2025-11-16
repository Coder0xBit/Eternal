#pragma once

#include <utils/Base.h>
#include <core/resource/Image.h>

namespace Eternal {
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
            m_AlbedoTexture = albedoTexture;
            if (m_AlbedoTexture) {
                m_PipelineLayoutBitMask = m_PipelineLayoutBitMask |= PipelineParams::SAMPLER;
            }
        }

        uint32_t getPipelineLayoutBitMask() const { return m_PipelineLayoutBitMask; }

        const Image* getAlbedoTexture() const { return m_AlbedoTexture; }

    private:
        Image* m_AlbedoTexture = nullptr;
        uint32_t m_PipelineLayoutBitMask = PipelineParams::UBO | PipelineParams::SAMPLER;
    };
}
