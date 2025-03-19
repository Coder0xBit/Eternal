#pragma once
#include <eternal/core/ImGuiLayer.h>
#include <eternal/core/graphics/vulkan/VulkanGraphicsContext.h>

namespace Eternal {
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public :
		VulkanImGuiLayer(VulkanGraphicsContext* vulkanGraphicsContext);

		~VulkanImGuiLayer();

		void beginFrame() override;

		void endFrame() override;	

	private :
		VulkanGraphicsContext* m_VulkanGraphicsContext = nullptr;
	};
}