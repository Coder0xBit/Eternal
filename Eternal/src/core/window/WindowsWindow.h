#pragma once

#include "core/window/Window.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>


namespace Eternal {
    constexpr const char* WINDOW_ICON_PATH = "res/PNGs/eternal_logo.png";

    class WindowsWindow : public Window {
    public:
        WindowsWindow(const Builder& builder);

        ~WindowsWindow() override;

        void onUpdate() override;

        virtual uint32_t getHeight() const override { return mData.height; }

        virtual uint32_t getWidth() const override { return mData.width; }

        virtual void* getNativeWindow() const override { return mWindow; }

        virtual void setCursorInputMode(CursorInputMode inputMode) override;

        virtual bool shouldClose() const override;

        virtual void shutDown() const override;

        bool isMinimized() const override { return mData.height == 0 || mData.width == 0; }

        virtual float getAspectRatio() const override {
            return static_cast<float>(mData.width) / static_cast<float>(mData.height);
        }

        virtual void setEventCallBack(const EventCallback& callback) override {
            mData.eventCallback = callback;
        }

        void setWindowIcon(const std::filesystem::path& path) const;

        virtual bool isKeyPressed(Eternal::KeyCode keycode) const override;

    private:
        GLFWwindow* mWindow = nullptr;

        struct WindowData {
            std::string title;
            uint32_t height = 0;
            uint32_t width = 0;
            Backend backend = Backend::Vulkan;
            bool VSync;

            EventCallback eventCallback;
        };

        WindowData mData;
    };
}
