#include "Editor.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include <eternal/core/graphics/vulkan/VulkanImGuiLayer.h>
#include <eternal/core/graphics/vulkan/VulkanGraphicsContext.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
namespace Eternal {

	Editor::Editor()
	{
		Eternal::Logger::Init();
		m_Engine = Eternal::Engine::Builder()
			.applicationName("Eternal Application")
			.build();

		Eternal::VulkanPlatform* platform = static_cast<Eternal::VulkanPlatform*>(m_Engine->getPlatform());
		m_ImGuiLayer = new VulkanImGuiLayer(platform);

		m_IsRunning = true;
	}

	Editor::~Editor()
	{
		shutdown();
	}

	void Editor::run()
	{
		while (m_IsRunning)
		{
			m_Engine->onUpdate();
			m_IsRunning = m_Engine->isRunning();
		}
	}

	Editor* Editor::create()
	{
		return new Editor();
	}

	void Editor::onImGuiRender()
	{
		static bool showDemoWindow = true;
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	void Editor::shutdown()
	{
		delete m_ImGuiLayer;
		delete m_Engine;
	}
}
