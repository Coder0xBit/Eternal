
namespace Eternal {
	class ImGuiApplication
	{
	public:
		virtual ~ImGuiApplication() = default;
		virtual void run() = 0;
		virtual void shutdown() = 0;
		virtual void onImGuiRender() = 0;

	protected:
		bool m_IsRunning = false;
	};
}