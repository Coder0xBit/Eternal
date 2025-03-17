#include <eternal/core/Logger.h>
#include <eternal/core/Engine.h>
#include "Application.h"

namespace Eternal {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Eternal::Logger::Init();
		m_Engine = Eternal::Engine::Builder()
			.applicationName("Eternal Application")
			.build();

		m_IsRunning = m_Engine->isRunning();
	}

	Application::~Application()
	{
		shutdown();
	}

	void Application::run()
	{
		while (m_IsRunning)
		{
			m_Engine->run();
			m_IsRunning = m_Engine->isRunning();
		}
	}

	void Application::shutdown()
	{
		delete m_Engine;
	}
}
