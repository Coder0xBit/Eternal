#include <eternal/core/Engine.h>
#include <eternal/core/ImGuiLayer.h>
#include <editor/ImGuiApplication.h>
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
		Engine* m_Engine = nullptr;
		ImGuiLayer* m_ImGuiLayer = nullptr;
	};
}