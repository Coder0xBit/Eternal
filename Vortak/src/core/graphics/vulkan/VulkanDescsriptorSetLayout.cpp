#include "core/graphics/vulkan/VulkanDescsriptorSetLayout.h"

namespace Vortak {

	VulkanDescriptorSetLayout::Builder::Builder(vk::Device logicalDevice) noexcept {
		mImpl->logicalDevice = logicalDevice;
	}

	VulkanDescriptorSetLayout::Builder::Builder(Builder const& rhs) noexcept = default;
	VulkanDescriptorSetLayout::Builder::Builder(Builder&& rhs) noexcept = default;
	VulkanDescriptorSetLayout::Builder::~Builder() noexcept = default;
	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::operator=(Builder const& rhs) noexcept = default;
	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::operator=(Builder&& rhs) noexcept = default;

	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::addBinding(LayoutInfo layoutInfo) {
		VORTAK_ASSERT(!mImpl->bindings.contains(layoutInfo.binding), "Binding is in use");
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
		mLogicalDevice = builder->logicalDevice;
		mBinding = builder->bindings;

		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		bindings.reserve(mBinding.size());

		for (const auto& [binding, layoutBinding] : mBinding)
			bindings.push_back(layoutBinding);

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
			.setBindings(bindings);

		mDescriptorSetLayout = mLogicalDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
		if (mDescriptorSetLayout) {
			mLogicalDevice.destroyDescriptorSetLayout(mDescriptorSetLayout);
			mDescriptorSetLayout = nullptr;
		}
		mBinding.clear();
	}
}