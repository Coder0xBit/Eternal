#include <eternal/core/Engine.h>
#include <eternal/core/ImGuiLayer.h>
#include <editor/ImGuiApplication.h>
#include <eternal/core/Window.h>

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
		void setupEntities();

		void addTriangle();

		void addCube();

		Engine* m_Engine = nullptr;

		EntityManager* m_EntityManager = nullptr;

		Window* m_Window = nullptr;

		SwapChain* m_SwapChain = nullptr;

		ImGuiLayer* m_ImGuiLayer = nullptr;
	};
}