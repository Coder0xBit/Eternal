#include "VulkanDescsriptorSetLayout.h"

namespace Eternal {

	VulkanDescriptorSetLayout::Builder::Builder(vk::Device logicalDevice) noexcept {
		mImpl->logicalDevice = logicalDevice;
	}

	VulkanDescriptorSetLayout::Builder::Builder(Builder const& rhs) noexcept = default;
	VulkanDescriptorSetLayout::Builder::Builder(Builder&& rhs) noexcept = default;
	VulkanDescriptorSetLayout::Builder::~Builder() noexcept = default;
	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::operator=(Builder const& rhs) noexcept = default;
	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::operator=(Builder&& rhs) noexcept = default;

	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::addBinding(LayoutInfo layoutInfo) {
		ETERNAL_ASSERT(mImpl->bindings.count(layoutInfo.binding) == 0, "Binding is in use");
		vk::DescriptorSetLayoutBinding binding = vk::DescriptorSetLayoutBinding()
			.setBinding(layoutInfo.binding)
			.setDescriptorType(layoutInfo.type)
			.setStageFlags(layoutInfo.stageFlags)
			.setDescriptorCount(layoutInfo.count);
		mImpl->bindings[layoutInfo.binding] = binding;
		return *this;
	}

	VulkanDescriptorSetLayout* VulkanDescriptorSetLayout::Builder::build() noexcept {
		return Memory::Allocate<VulkanDescriptorSetLayout>(*this);
	}

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Builder& builder) {
		m_LogicalDevice = builder->logicalDevice;
		m_Binding = builder->bindings;

		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		bindings.reserve(m_Binding.size());

		for (const auto& [binding, layoutBinding] : m_Binding)
			bindings.push_back(layoutBinding);

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
			.setBindings(bindings);

		m_DescriptorSetLayout = m_LogicalDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
		if (m_DescriptorSetLayout) {
			m_LogicalDevice.destroyDescriptorSetLayout(m_DescriptorSetLayout);
			m_DescriptorSetLayout = nullptr;
		}
		m_Binding.clear();
	}
}