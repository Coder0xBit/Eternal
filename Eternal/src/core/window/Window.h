#pragma once

#include "utils/Base.h"
#include "core/event/Event.h"
#include "core/input/KeyCodes.h"

#include <vulkan/vulkan.hpp>

namespace Eternal {
    enum CursorInputMode {
        LOCKED,
        NORMAL
    };

    class Window {
    public:
        using EventCallback = std::function<void(Event&)>;

        virtual ~Window() = default;
        virtual void onUpdate() = 0;
        virtual uint32_t getHeight() const = 0;
        virtual uint32_t getWidth() const = 0;
        virtual float getAspectRatio() const = 0;
        virtual void* getNativeWindow() const = 0;
        virtual void setCursorInputMode(CursorInputMode inputMode) = 0;
        virtual bool shouldClose() const = 0;
        virtual void shutDown() const = 0;
        virtual bool isMinimized() const = 0;
        virtual void setEventCallBack(const EventCallback& callback) = 0;
        virtual bool isKeyPressed(Eternal::KeyCode keycode) const = 0;

        struct BuilderDetails {
            std::string title = "";
            uint32_t height = 0;
            uint32_t width = 0;
        };

        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class Window;
            friend class WindowsWindow;

        public:
            Builder() noexcept;
            Builder(Builder const& rhs) noexcept;
            Builder(Builder&& rhs) noexcept;
            ~Builder() noexcept;
            Builder& operator=(Builder const& rhs) noexcept;
            Builder& operator=(Builder&& rhs) noexcept;
            Builder& title(const std::string& title) noexcept;
            Builder& width(uint32_t witdth) noexcept;
            Builder& height(uint32_t height) noexcept;
            std::unique_ptr<Window> build() const noexcept;
        };

        static std::unique_ptr<Window> create(const Builder& builder);

    protected:
        bool m_IsWindowResized = false;
    };
}
