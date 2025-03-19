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
		Eternal::VulkanGraphicsContext* context = platform->getContext();
		m_ImGuiLayer = new VulkanImGuiLayer(context);

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
			m_ImGuiLayer->beginFrame();
			onImGuiRender();
			m_ImGuiLayer->endFrame();
		}
	}

	Editor* Editor::create()
	{
		return new Editor();
	}

	void Editor::onImGuiRender()
	{
		static bool show_demo_window = true;
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	void Editor::shutdown()
	{
		delete m_Engine;
	}
}
