#pragma once
#include <core/graphics/SwapChain.h>
#include <core/graphics/FrameInfo.h>

#include <utils/Base.h>

namespace Eternal {
	class Renderer {
	public:
		virtual FrameInfo* beginFrame() = 0;

		virtual void render() = 0;

		virtual void endFrame() = 0;

		virtual ~Renderer() = default;
	};
}