#pragma once
#include <core/graphics/FrameInfo.h>

namespace Eternal {
	class ImGuiLayer
	{
	public:
		virtual void beginFrame() = 0;

		virtual void render(FrameInfo* frameInfo) = 0;

		virtual ~ImGuiLayer() = default;
	};
}