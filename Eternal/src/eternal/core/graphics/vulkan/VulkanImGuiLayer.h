#pragma once
#include <eternal/core/ImGuiLayer.h>
#include <eternal/core/graphics/vulkan/VulkanGraphicsContext.h>
#include <eternal/core/graphics/vulkan/VulkanPlatform.h>
#include <eternal/core/graphics/vulkan/VulkanSwapChain.h>
#include <eternal/core/Window.h>

namespace Eternal {
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer(VulkanPlatform* vulkanPlatform, VulkanSwapChain* swapChain, Window* window);

		~VulkanImGuiLayer();

		void beginFrame() override;

		void render() override;

	private:
		VulkanPlatform* m_VulkanPlatform = nullptr;

		VulkanSwapChain* m_SwapChain = nullptr;

		Window* m_Window = nullptr;

		void init();
	};
}