#include "core/window/Window.h"
#include <core/window/WindowsWindow.h>
#include <core/System.h>

namespace Eternal {
    std::unique_ptr<Window> Window::create(const Builder& builder) {
        System system = Eternal::detectSystem();

        switch (system) {
            case System::WINDOWS:
                return std::make_unique<WindowsWindow>(builder);
                break;
            default:
                return nullptr;
                break;
        }
    }

    Window::Builder::Builder() noexcept = default;
    Window::Builder::Builder(Builder const& rhs) noexcept = default;
    Window::Builder::Builder(Builder&& rhs) noexcept = default;
    Window::Builder::~Builder() noexcept = default;
    Window::Builder& Window::Builder::operator=(Builder const& rhs) noexcept = default;
    Window::Builder& Window::Builder::operator=(Builder&& rhs) noexcept = default;

    Window::Builder& Window::Builder::title(const std::string& title) noexcept {
        mImpl->title = title;
        return *this;
    }

    Window::Builder& Window::Builder::height(uint32_t height) noexcept {
        mImpl->height = height;
        return *this;
    }

    Window::Builder& Window::Builder::width(uint32_t width) noexcept {
        mImpl->width = width;
        return *this;
    }

    std::unique_ptr<Window> Window::Builder::build() const noexcept {
        return Window::create(*this);
    }
}
