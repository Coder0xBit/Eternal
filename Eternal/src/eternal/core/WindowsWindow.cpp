
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "WindowsWindow.h"

namespace Eternal {

	WindowsWindow::WindowsWindow(const Builder& builder) :
		m_Title(builder->title),
		m_Height(builder->height),
		m_Width(builder->width)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

		setWindowIcon(WINDOW_ICON_PATH, m_Window);

		Eternal::Logger::Info("Window Created");
	}

	void WindowsWindow::onUpdate() {
		glfwPollEvents();
	}

	bool WindowsWindow::shouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void WindowsWindow::shutDown() const
	{
		if (m_Window != nullptr)
		{
			glfwDestroyWindow(m_Window);
		}
		else
		{
			Eternal::Logger::Error("mWindow Pointer is null");
		}

		glfwTerminate();

		Eternal::Logger::Info("Window Destroyed");
	}

	vk::SurfaceKHR WindowsWindow::createWindowSurface(vk::Instance instance) const
	{
		ETERNAL_ASSERT(m_Window != nullptr, "Window is null");

		vk::SurfaceKHR surface = nullptr;
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS)
		{
			Eternal::Logger::Error("Failed to create window surface");
			return nullptr;
		}
		return surface;
	}

	vk::Extent2D WindowsWindow::getExtent() const
	{
		ETERNAL_ASSERT(m_Window != nullptr, "Window is null");
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_Window, &width, &height);
		return { static_cast<uint32_t>(width),static_cast<uint32_t>(height) };
	}

	void WindowsWindow::setWindowIcon(const std::filesystem::path& path, GLFWwindow* window)
	{
		if (window == nullptr)
		{
			Eternal::Logger::Error("Trying to set icon on null window (GLFW)");
			return;
		}

		GLFWimage icon;
		int width, height, channels;

		icon.pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
		if (!icon.pixels)
		{
			Eternal::Logger::Error("Failed to load icon, from this path {}", path.string());
			return;
		}

		icon.width = width;
		icon.height = height;

		glfwSetWindowIcon(window, 1, &icon);

		stbi_image_free(icon.pixels);
	}

	WindowsWindow::~WindowsWindow()
	{
		shutDown();
	}
}
