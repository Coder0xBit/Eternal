#include "WindowsWindow.h"

#include "event/KeyEvents.h"
#include "event/MouseEvents.h"
#include "event/WindowEvent.h"
#include "resource/ResourceManager.h"
#include "resource/Image.h"

namespace Eternal {
    WindowsWindow::WindowsWindow(const Builder& builder) {
        m_Data.title = builder->title;
        m_Data.height = builder->height;
        m_Data.width = builder->width;

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


        m_Window = glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
        ETERNAL_ASSERT(m_Window != nullptr, "Failed to create GLFW window");

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            const WindowData& windowData = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));\
            WindowResizeEvent event(width, height);
            windowData.eventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            const WindowData& windowData = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));
            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    windowData.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    windowData.eventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, true);
                    windowData.eventCallback(event);
                    break;
                }
                default: break;
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
            WindowData& windowData = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));
            MouseMovedEvent event(xpos, ypos);
            windowData.eventCallback(event);
        });

        Logger::Info("Current Working Path {} , Accessing Window Icon", std::filesystem::current_path().string());
        setWindowIcon(WINDOW_ICON_PATH, m_Window);

        Eternal::Logger::Info("Window Created");
    }

    void WindowsWindow::onUpdate() {
        glfwPollEvents();
    }

    bool WindowsWindow::shouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    void WindowsWindow::shutDown() const {
        if (m_Window != nullptr) {
            glfwDestroyWindow(m_Window);
        } else {
            Eternal::Logger::Error("mWindow Pointer is null");
        }

        glfwTerminate();

        Eternal::Logger::Info("Window Destroyed");
    }

    vk::SurfaceKHR WindowsWindow::createWindowSurface(vk::Instance instance) const {
        ETERNAL_ASSERT(m_Window != nullptr, "Window is null");

        vk::SurfaceKHR surface = nullptr;
        if (glfwCreateWindowSurface(instance, m_Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) !=
            VK_SUCCESS) {
            Eternal::Logger::Error("Failed to create window surface");
            return nullptr;
        }
        return surface;
    }

    vk::Extent2D WindowsWindow::getExtent() const {
        ETERNAL_ASSERT(m_Window != nullptr, "Window is null");
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    void WindowsWindow::setWindowIcon(const std::filesystem::path& path, GLFWwindow* window) {
        if (window == nullptr) {
            Eternal::Logger::Error("Trying to set icon on null window (GLFW)");
            return;
        }

        Image* image = ResourceManager::get().loadResource<Image>(path.string());
        GLFWimage icon = image->getGLFWImage();
        glfwSetWindowIcon(window, 1, &icon);
    }

    bool WindowsWindow::isKeyPressed(Eternal::KeyCode keycode) const {
        auto state = glfwGetKey(m_Window, keycode);
        return state == GLFW_PRESS;
    }

    WindowsWindow::~WindowsWindow() {
        WindowsWindow::shutDown();
    }
}
