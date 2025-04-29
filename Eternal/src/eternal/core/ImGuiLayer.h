#pragma once

namespace Eternal {
	class ImGuiLayer
	{
	public:
		virtual void beginFrame() = 0;

		virtual void render() = 0;

		virtual ~ImGuiLayer() = default;
	};
}