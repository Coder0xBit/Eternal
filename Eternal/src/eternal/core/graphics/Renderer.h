#pragma once
#include <eternal/core/graphics/SwapChain.h>
#include <eternal/utils/Base.h>

namespace Eternal {
	class Renderer {
	public:
		struct FrameInfo {
			virtual ~FrameInfo() = default;
		};

		virtual FrameInfo* beginFrame() = 0;

		virtual void render(FrameInfo* frameInfo) = 0;

		virtual void endFrame() = 0;

		virtual ~Renderer() = default;
	};
}