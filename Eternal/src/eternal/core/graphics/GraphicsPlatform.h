#pragma once

namespace Eternal {
	class GraphicsPlatform
	{
	public:
		virtual ~GraphicsPlatform() = default;
		virtual void initialize() = 0;
		virtual void shutDown() = 0;
		virtual void render() = 0;
	};
}