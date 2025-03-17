#pragma once

#include <chrono>

namespace Eternal {

	class Timer {
	public:
		Timer()
		{
			lastFrameTime = std::chrono::high_resolution_clock::now();
		}

		float getDeltaTime()
		{
			auto deltaTime = getDeltaDuration();
			return deltaTime.count();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

		std::chrono::duration<float> getDeltaDuration() {
			auto currentTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> deltaTime = currentTime - lastFrameTime;
			lastFrameTime = currentTime;
			return deltaTime;
		}
	};
}