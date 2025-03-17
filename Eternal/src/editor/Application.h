#pragma once
#include <eternal/core/Engine.h>

namespace Eternal {
	class Application
	{
	public:
		Application(const Application& other) = delete;

		Application(const Application&& other) = delete;

		~Application();

		static Application* get()
		{
			if (!s_Instance)
				s_Instance = new Application();

			return s_Instance;
		}

		void run();

		void shutdown();

	private:
		Application();

		static Application* s_Instance;

		Engine* m_Engine = nullptr;

		bool m_IsRunning = false;
	};
}