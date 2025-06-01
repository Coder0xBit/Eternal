#include <core/Engine.h>
#include <core/ImGuiLayer.h>
#include <core/Window.h>
#include <core/scene/Scene.h>

#include "ImGuiApplication.h"

namespace Eternal {
	class Editor : public ImGuiApplication
	{
	public:
		Editor();

		~Editor();

		static Editor* create();

		void onImGuiRender() override;

		void run() override;

		void shutdown() override;

	private:
		void addEntity(std::string name);

		void addEntity(std::string name , std::string filePath , glm::vec3 initialPosition);

		void addTriangle();

		void addCube();

		Engine* m_Engine = nullptr;

		Eternal::Scene* m_Scene = nullptr;

		Eternal::Renderer* m_Renderer = nullptr;

		Window* m_Window = nullptr;

		SwapChain* m_SwapChain = nullptr;

		ImGuiLayer* m_ImGuiLayer = nullptr;

	};
}