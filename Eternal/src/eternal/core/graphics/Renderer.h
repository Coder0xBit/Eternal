#pragma once
#include <eternal/core/graphics/SwapChain.h>
#include <eternal/core/graphics/FrameInfo.h>

#include <eternal/utils/Base.h>

namespace Eternal {
	class Renderer {
	public:
		virtual FrameInfo* beginFrame() = 0;

		virtual void render() = 0;

		virtual void endFrame() = 0;

		virtual ~Renderer() = default;
	};
}