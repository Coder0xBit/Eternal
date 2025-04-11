#pragma once

#include <eternal/core/graphics/vulkan/VulkanWindow.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <filesystem>

namespace Eternal {

	constexpr const char* WINDOW_ICON_PATH = "res/PNGs/eternal_logo.png";

	class WindowsWindow : public VulkanWindow
	{
	public:
		WindowsWindow(const Builder& builder);

		~WindowsWindow();

		virtual void onUpdate() override;

		virtual uint32_t getHeight() const override { return m_Height; }

		virtual uint32_t getWidth() const override { return m_Width; }

		virtual void* getNativeWindow() const override { return m_Window; }

		virtual bool shouldClose() const override;

		virtual void shutDown() const override;

		virtual vk::SurfaceKHR createWindowSurface(vk::Instance instance) const override;

		virtual vk::Extent2D getExtent() const override;

		void setWindowIcon(const std::filesystem::path& path, GLFWwindow* window);

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_Title = "";
		uint32_t m_Height = 0;
		uint32_t m_Width = 0;
	};
}