#include <core/Engine.h>
#include <core/ImGuiLayer.h>
#include <core/Window.h>
#include <core/scene/Scene.h>

#include "ImGuiApplication.h"
#include "core/graphics/Camera.hpp"

namespace Eternal {

	struct TestEntityDetails {
		std::string name = "test_entity";
		std::string filePath = "res/models/cube.obj";
		std::string texturePath = "";
		glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	class Editor : public ImGuiApplication {
	public:
		Editor();
		~Editor();

		static Editor* create();

		void onImGuiRender() override;
		void run() override;
		void shutdown() override;

	private:
		void addEntity(TestEntityDetails testEntity);
		void addTriangle();
		void addCube();

		Engine* m_Engine = nullptr;
		Eternal::Scene* m_Scene = nullptr;
		Eternal::Renderer* m_Renderer = nullptr;
		Eternal::Camera* m_EditorCamera = nullptr;
		Window* m_Window = nullptr;
		SwapChain* m_SwapChain = nullptr;
		ImGuiLayer* m_ImGuiLayer = nullptr;

	};
}
