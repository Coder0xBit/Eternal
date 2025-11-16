#pragma once

#include <core/graphics/vulkan/VulkanUtils.h>
#include <core/graphics/vulkan/VulkanDescsriptorSetLayout.h>
#include <utils/Base.h>

namespace Eternal {

	constexpr uint32_t MAX_DESCRIPTOR_POOL_SIZE = 100;

	class VulkanDescriptorPool {
	public:
		struct PoolSize {
			vk::DescriptorType type;
			uint32_t descriptorCount;
		};

		struct BuilderDetails {
			vk::Device logicalDevice;
			std::vector<PoolSize> poolSizes;
			uint32_t maxSets = MAX_DESCRIPTOR_POOL_SIZE;
		};

		class Builder : public utils::PrivateImplementation<BuilderDetails> {
			friend class VulkanDescriptorPool;
		public:
			Builder(vk::Device logicalDevice) noexcept;
			Builder(Builder const& rhs) noexcept;
			Builder(Builder&& rhs) noexcept;
			~Builder() noexcept;
			Builder& operator=(Builder const& rhs) noexcept;
			Builder& operator=(Builder&& rhs) noexcept;
			Builder& addPoolSize(PoolSize poolSize);
			Builder& setMaxSets(uint32_t maxSets) noexcept;
			VulkanDescriptorPool* build() noexcept;
		};

		VulkanDescriptorPool(const Builder& builder);
		VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;
		~VulkanDescriptorPool();

		vk::DescriptorPool getDescriptorPool() const { return m_DescriptorPool; }
		void reset();
		vk::DescriptorSet allocate(const VulkanDescriptorSetLayout& descriptorSetLayout);
		std::vector<vk::DescriptorSet> allocate(uint32_t descriptorSetCount, const VulkanDescriptorSetLayout& descriptorSetLayout);

		void free(vk::DescriptorSet* descriptorSet);
		void free(const std::vector<vk::DescriptorSet>& descriptorSets);

		const std::vector<vk::DescriptorPoolSize>& getPoolSizes() const { return m_PoolSizes; }

	private:
		vk::Device m_LogicalDevice;
		vk::DescriptorPool m_DescriptorPool;

		std::vector<vk::DescriptorPoolSize> m_PoolSizes;

		uint32_t m_MaxSets;
	};
}