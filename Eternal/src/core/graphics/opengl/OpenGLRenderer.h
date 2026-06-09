#pragma once
#include "core/graphics/Renderer.h"
#include "core/graphics/BufferManager.h"

namespace Eternal {
    class OpenGLRenderer : public Renderer {
    public:
        FrameInfo* beginFrame() override;
        void render(Eternal::Camera* camera) override;
        void endFrame() override;
        SwapChain* getSwapChain() const override;

    private :
        Eternal::BufferManager* mBufferManager = nullptr;
    };
}
