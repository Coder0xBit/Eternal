#pragma once

#include <eternal/core/graphics/vulkan/VulkanUtils.h>
#include <eternal/utils/Base.h>

namespace Eternal {

	using VulkanDesccriptorBinding = std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>;

	class VulkanDescriptorSetLayout {
	public:
		struct LayoutInfo {
			uint32_t binding;
			vk::DescriptorType type;
			vk::ShaderStageFlags stageFlags;
			uint32_t count;
		};

		struct BuilderDetails {
			vk::Device logicalDevice;
			VulkanDesccriptorBinding bindings;
		};

		class Builder : public utils::PrivateImplementation<BuilderDetails> {
			friend class VulkanDescriptorSetLayout;
		public:
			Builder(vk::Device logicalDevice) noexcept;

			Builder(Builder const& rhs) noexcept;

			Builder(Builder&& rhs) noexcept;

			~Builder() noexcept;

			Builder& operator=(Builder const& rhs) noexcept;

			Builder& operator=(Builder&& rhs) noexcept;

			Builder& addBinding(LayoutInfo layoutInfo) noexcept;

			VulkanDescriptorSetLayout* build() noexcept;
		};

		VulkanDescriptorSetLayout(const Builder& builder);

		vk::DescriptorSetLayout getDescriptorSetLayout() const { return m_DescriptorSetLayout; }

		~VulkanDescriptorSetLayout();

	private:
		vk::Device m_LogicalDevice;

		VulkanDesccriptorBinding m_Binding;

		vk::DescriptorSetLayout m_DescriptorSetLayout;
	};
}