#include "core/window/WindowsWindow.h"
#include "core/event/KeyEvents.h"
#include "core/event/MouseEvents.h"
#include "core/event/WindowEvent.h"
#include "core/resource/ResourceManager.h"
#include "core/resource/Image.h"

namespace Vortak {
    WindowsWindow::WindowsWindow(const Builder& builder) {
        mData.title = builder->title;
        mData.height = builder->height;
        mData.width = builder->width;
        mData.backend = builder->backend;

        glfwInit();

        if (mData.backend == Backend::Vulkan) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        } else {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


        mWindow = glfwCreateWindow(mData.width, mData.height, mData.title.c_str(), nullptr, nullptr);
        VORTAK_ASSERT(mWindow != nullptr, "Failed to create GLFW window");

        glfwMakeContextCurrent(mWindow);

        glfwSetWindowUserPointer(mWindow, &mData);

        // glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
            const WindowData& windowData = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));\
            WindowResizeEvent event(width, height);
            windowData.eventCallback(event);
        });

        glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
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

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xpos, double ypos) {
            WindowData& windowData = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));
            MouseMovedEvent event(xpos, ypos);
            windowData.eventCallback(event);
        });

        Logger::Info("Current Working Path {} , Accessing Window Icon", std::filesystem::current_path().string());
        setWindowIcon(WINDOW_ICON_PATH);

        Vortak::Logger::Info("Window Created");
    }

    void WindowsWindow::onUpdate() {
        glfwPollEvents();
    }

    void WindowsWindow::setCursorInputMode(CursorInputMode inputMode) {
        int cursorInputMode;
        if (inputMode == CursorInputMode::LOCKED) {
            cursorInputMode = GLFW_CURSOR_DISABLED;
        } else {
            cursorInputMode = GLFW_CURSOR_NORMAL;
        }
        glfwSetInputMode(mWindow, GLFW_CURSOR, cursorInputMode);
    }

    bool WindowsWindow::shouldClose() const {
        return glfwWindowShouldClose(mWindow);
    }

    void WindowsWindow::shutDown() const {
        if (mWindow != nullptr) {
            glfwDestroyWindow(mWindow);
        } else {
            Vortak::Logger::Error("mWindow Pointer is null");
        }

        glfwTerminate();

        Vortak::Logger::Info("Window Destroyed");
    }

    void WindowsWindow::setWindowIcon(const std::filesystem::path& path) const {
        if (mWindow == nullptr) {
            Vortak::Logger::Error("Trying to set icon on null mWindow (GLFW)");
            return;
        }

        Image* image = ResourceManager::get().loadResource<Image>(path.string());
        GLFWimage icon = image->getGLFWImage();
        glfwSetWindowIcon(mWindow, 1, &icon);
    }

    bool WindowsWindow::isKeyPressed(Vortak::KeyCode keycode) const {
        auto state = glfwGetKey(mWindow, keycode);
        return state == GLFW_PRESS;
    }

    WindowsWindow::~WindowsWindow() {
        WindowsWindow::shutDown();
    }
}
