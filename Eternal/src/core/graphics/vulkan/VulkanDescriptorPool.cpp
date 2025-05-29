#include "VulkanDescriptorPool.h"

namespace Eternal {
	VulkanDescriptorPool::Builder::Builder(vk::Device logicalDevice) noexcept
	{
		mImpl->logicalDevice = logicalDevice;
	}

	VulkanDescriptorPool::Builder::Builder(Builder const& rhs) noexcept = default;

	VulkanDescriptorPool::Builder::Builder(Builder&& rhs) noexcept = default;

	VulkanDescriptorPool::Builder::~Builder() noexcept = default;

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::operator=(Builder const& rhs) noexcept = default;

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::operator=(Builder&& rhs) noexcept = default;

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::addPoolSize(PoolSize poolSize)
	{
		ETERNAL_ASSERT(mImpl->poolSizes.size() < MAX_DESCRIPTOR_POOL_SIZE, "Pool size limit reached, Seriosuly Brah!");
		mImpl->poolSizes.push_back(poolSize);
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::setMaxSets(uint32_t maxSets) noexcept
	{
		mImpl->maxSets = maxSets;
		return *this;
	}

	VulkanDescriptorPool* VulkanDescriptorPool::Builder::build() noexcept
	{
		return Memory::Allocate<VulkanDescriptorPool>(*this);
	}

	VulkanDescriptorPool::VulkanDescriptorPool(const Builder& builder)
	{
		m_LogicalDevice = builder->logicalDevice;
		m_MaxSets = builder->maxSets;

		m_PoolSizes.reserve(m_PoolSizes.size());

		for (const auto& poolSize : builder->poolSizes)
		{
			vk::DescriptorPoolSize descriptorPoolSize = vk::DescriptorPoolSize()
				.setType(poolSize.type)
				.setDescriptorCount(poolSize.descriptorCount);
			m_PoolSizes.push_back(descriptorPoolSize);
		}

		vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
			.setMaxSets(m_MaxSets)
			.setPoolSizes(m_PoolSizes);

		m_DescriptorPool = m_LogicalDevice.createDescriptorPool(descriptorPoolCreateInfo);
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		if (m_DescriptorPool)
		{
			m_LogicalDevice.destroyDescriptorPool(m_DescriptorPool);
			m_DescriptorPool = nullptr;
		}
		m_PoolSizes.clear();
		m_CurrentlyAllocatedSets = 0;
	}

	void VulkanDescriptorPool::reset()
	{
		m_LogicalDevice.resetDescriptorPool(m_DescriptorPool, vk::DescriptorPoolResetFlags());
		m_CurrentlyAllocatedSets = 0;
	}

	vk::DescriptorSet VulkanDescriptorPool::allocate(const VulkanDescriptorSetLayout& descriptorSetLayout)
	{
		auto descriptorLayout = descriptorSetLayout.getDescriptorSetLayout();

		vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(m_DescriptorPool)
			.setDescriptorSetCount(1)
			.setSetLayouts(descriptorLayout);

		vk::DescriptorSet descriptorSet = m_LogicalDevice.allocateDescriptorSets(descriptorSetAllocateInfo)[0];
		m_CurrentlyAllocatedSets++;
		return descriptorSet;
	}

	std::vector<vk::DescriptorSet> VulkanDescriptorPool::allocate(uint32_t descriptorSetCount, const VulkanDescriptorSetLayout& descriptorSetLayout)
	{
		ETERNAL_ASSERT(descriptorSetCount <= m_MaxSets, "Descriptor set count exceeds max sets");

		std::vector<vk::DescriptorSetLayout> descriptorLayouts(descriptorSetCount, descriptorSetLayout.getDescriptorSetLayout());

		vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(m_DescriptorPool)
			.setDescriptorSetCount(descriptorSetCount)
			.setSetLayouts(descriptorLayouts);

		auto descSets = m_LogicalDevice.allocateDescriptorSets(descriptorSetAllocateInfo);

		ETERNAL_ASSERT(descSets.size() == descriptorSetCount, "Failed to allocate descriptor sets");

		m_CurrentlyAllocatedSets += descriptorSetCount;

		return descSets;
	}

	void VulkanDescriptorPool::free(vk::DescriptorSet* descriptorSet)
	{
		m_LogicalDevice.freeDescriptorSets(m_DescriptorPool, 1, descriptorSet);
		m_CurrentlyAllocatedSets--;
	}

	void VulkanDescriptorPool::free(const std::vector<vk::DescriptorSet>& descriptorSets)
	{
		m_CurrentlyAllocatedSets -= descriptorSets.size();
		m_LogicalDevice.freeDescriptorSets(m_DescriptorPool, descriptorSets.size(), descriptorSets.data());
	}

}