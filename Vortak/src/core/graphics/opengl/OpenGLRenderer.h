#pragma once
#include "core/graphics/Renderer.h"
#include "core/graphics/BufferManager.h"

namespace Vortak {
    class OpenGLRenderer : public Renderer {
    public:
        FrameInfo* beginFrame() override;
        void render(Vortak::Camera* camera) override;
        void endFrame() override;
        SwapChain* getSwapChain() const override;

    private :
        std::unique_ptr<BufferManager> mBufferManager = nullptr;
    };
}
