#pragma once

namespace Vortak {
	class Application
	{
	public:
		virtual ~Application() = default;
		virtual void run() = 0;
		virtual void shutdown() = 0;

	protected:
		bool mIsRunning = false;
	};
}