
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "WindowsWindow.h"

namespace Eternal {

	WindowsWindow::WindowsWindow(const Builder& builder) :
		m_Title(builder->mTitle),
		m_Height(builder->mHeight),
		m_Width(builder->mWidth)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
		setWindowIcon(WINDOW_ICON_PATH, m_Window);
		Logger::Info("Window Created");
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
			Logger::Error("mWindow Pointer is null");
		}

		glfwTerminate();
		Logger::Info("Window Destroyed");
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
