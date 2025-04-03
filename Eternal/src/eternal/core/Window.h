#pragma once
#include <eternal/utils/Base.h>

namespace Eternal {

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t getHeight() const = 0;

		virtual uint32_t getWidth() const = 0;

		virtual void* getNativeWindow() const = 0;

		virtual bool shouldClose() const = 0;

		virtual void shutDown() const = 0;

		struct BuilderDetails {
			std::string mTitle = "";
			uint32_t mHeight = 0;
			uint32_t mWidth = 0;
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

			Window* build() noexcept;
		};

		static Window* create(const Builder& builder);
	};
}
