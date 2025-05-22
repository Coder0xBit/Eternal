#pragma once

#include <core/Window.h>

#include <utils/Base.h>
#include <vulkan/vulkan.hpp>

namespace Eternal {
	class VulkanWindow : public Window
	{
	public :
		virtual ~VulkanWindow() = default;

		virtual vk::SurfaceKHR createWindowSurface(vk::Instance instance) const = 0;

		virtual vk::Extent2D getExtent() const = 0;
	};
}