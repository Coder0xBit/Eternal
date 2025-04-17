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

		Memory::Ref<Engine> m_Engine = nullptr;
		Memory::Ref<Window> m_Window = nullptr;
		Memory::Ref<SwapChain> m_SwapChain = nullptr;
		Memory::Ref<EntityManager> m_EntityManager = nullptr;
		ImGuiLayer* m_ImGuiLayer = nullptr;
	};
}