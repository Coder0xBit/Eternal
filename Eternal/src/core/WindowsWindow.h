#pragma once

#include <core/graphics/vulkan/VulkanWindow.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <filesystem>

namespace Eternal {

	constexpr const char* WINDOW_ICON_PATH = "res/PNGs/eternal_logo.png";

	class WindowsWindow : public VulkanWindow {
	public:
		WindowsWindow(const Builder& builder);
		~WindowsWindow();
		virtual void onUpdate() override;
		virtual uint32_t getHeight() const override { return m_Height; }
		virtual uint32_t getWidth() const override { return m_Width; }
		virtual void* getNativeWindow() const override { return m_Window; }
		virtual bool shouldClose() const override;
		virtual void shutDown() const override;
		virtual bool isMinimized() const { return m_Width == 0 || m_Height == 0; }
		virtual float getAspectRatio() const override {
			return static_cast<float>(m_Width) / static_cast<float>(m_Height);
		}
		virtual vk::SurfaceKHR createWindowSurface(vk::Instance instance) const override;
		virtual vk::Extent2D getExtent() const override;
		void setWindowIcon(const std::filesystem::path& path, GLFWwindow* window);

	private:
		void onWindowResize(GLFWwindow* window, int width, int height);

		GLFWwindow* m_Window = nullptr;
		std::string m_Title = "";
		uint32_t m_Height = 0;
		uint32_t m_Width = 0;
	};
}