#include <eternal/core/Window.h>

#include <eternal/core/WindowsWindow.h>
#include <eternal/core/System.h>

namespace Eternal {

	Window* Window::create(const Builder& builder)
	{
		System system = Eternal::detectSystem();

		switch (system)
		{
		case System::WINDOWS:
			return new WindowsWindow(builder);
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

	Window* Window::Builder::build() noexcept {
		return Window::create(*this);
	}
}


