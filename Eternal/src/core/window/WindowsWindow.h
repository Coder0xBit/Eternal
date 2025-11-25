#pragma once

#include "core/graphics/vulkan/VulkanWindow.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>


namespace Eternal {
    constexpr const char* WINDOW_ICON_PATH = "res/PNGs/eternal_logo.png";

    class WindowsWindow : public VulkanWindow {
    public:
        WindowsWindow(const Builder& builder);

        ~WindowsWindow() override;

        void onUpdate() override;

        virtual uint32_t getHeight() const override { return m_Data.height; }

        virtual uint32_t getWidth() const override { return m_Data.width; }

        virtual void* getNativeWindow() const override { return m_Window; }

        virtual bool shouldClose() const override;

        virtual void shutDown() const override;

        bool isMinimized() const override { return m_Data.height == 0 || m_Data.width == 0; }

        virtual float getAspectRatio() const override {
            return static_cast<float>(m_Data.width) / static_cast<float>(m_Data.height);
        }

        virtual void setEventCallBack(const EventCallback& callback) override {
            m_Data.eventCallback = callback;
        }

        virtual vk::SurfaceKHR createWindowSurface(vk::Instance instance) const override;

        virtual vk::Extent2D getExtent() const override;

        void setWindowIcon(const std::filesystem::path& path, GLFWwindow* window);

        virtual bool isKeyPressed(Eternal::KeyCode keycode) const override;

    private:
        GLFWwindow* m_Window = nullptr;

        struct WindowData {
            std::string title;
            uint32_t height = 0;
            uint32_t width = 0;
            bool VSync;

            EventCallback eventCallback;
        };

        WindowData m_Data;
    };
}
