#pragma once
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR

#include <eternal/utils/Base.h>
#include <eternal/core/Logger.h>
#include <eternal/core/Window.h>
#include <eternal/core/graphics/GraphicsPlatform.h>
#include <GLFW/glfw3.h>

namespace Eternal {


	class Engine
	{

	public:
		struct BuilderDetails {
			std::string applicationName;
		};

		class Builder : public utils::PrivateImplementation<BuilderDetails> {
			friend class Engine;

		public:
			Builder() noexcept;

			Builder(Builder const& rhs) noexcept;

			Builder(Builder&& rhs) noexcept;

			~Builder() noexcept;

			Builder& operator=(Builder const& rhs) noexcept;

			Builder& operator=(Builder&& rhs) noexcept;

			Builder& applicationName(const std::string& applicationName) noexcept;

			Engine* build();
		};

		Engine(const Builder& builder);

		~Engine();

		bool isRunning() const { return m_IsRunning; }

		void run();

	private:
		std::string m_ApplicationName;
		Eternal::Window* m_Window = nullptr;
		Eternal::GraphicsPlatform* m_GraphicsPlatform = nullptr;
		bool m_IsRunning = false;
	};
}