#include "core/graphics/vulkan/VulkanDescriptorPool.h"

namespace Eternal {
    VulkanDescriptorPool::Builder::Builder(vk::Device logicalDevice) noexcept {
        mImpl->logicalDevice = logicalDevice;
    }

    VulkanDescriptorPool::Builder::Builder(Builder const& rhs) noexcept = default;

    VulkanDescriptorPool::Builder::Builder(Builder&& rhs) noexcept = default;

    VulkanDescriptorPool::Builder::~Builder() noexcept = default;

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::operator=(Builder const& rhs) noexcept = default;

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::operator=(Builder&& rhs) noexcept = default;

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::addPoolSize(PoolSize poolSize) {
        ETERNAL_ASSERT(mImpl->poolSizes.size() < MAX_DESCRIPTOR_POOL_SIZE, "Pool size limit reached, Seriosuly Brah!");
        mImpl->poolSizes.push_back(poolSize);
        return *this;
    }

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::setMaxSets(uint32_t maxSets) noexcept {
        mImpl->maxSets = maxSets;
        return *this;
    }

    VulkanDescriptorPool* VulkanDescriptorPool::Builder::build() noexcept {
        return Memory::Allocate<VulkanDescriptorPool>(*this);
    }

    VulkanDescriptorPool::VulkanDescriptorPool(const Builder& builder) {
        mLogicalDevice = builder->logicalDevice;
        mMaxSets = builder->maxSets;

        mPoolSizes.reserve(mPoolSizes.size());

        for (const auto& poolSize: builder->poolSizes) {
            vk::DescriptorPoolSize descriptorPoolSize = vk::DescriptorPoolSize()
                    .setType(poolSize.type)
                    .setDescriptorCount(poolSize.descriptorCount);
            mPoolSizes.push_back(descriptorPoolSize);
        }

        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
                .setMaxSets(mMaxSets)
                .setPoolSizes(mPoolSizes);

        mDescriptorPool = mLogicalDevice.createDescriptorPool(descriptorPoolCreateInfo);
    }

    VulkanDescriptorPool::~VulkanDescriptorPool() {
        if (mDescriptorPool) {
            mLogicalDevice.destroyDescriptorPool(mDescriptorPool);
            mDescriptorPool = nullptr;
        }
        mPoolSizes.clear();
    }

    void VulkanDescriptorPool::reset() {
        mLogicalDevice.resetDescriptorPool(mDescriptorPool, vk::DescriptorPoolResetFlags());
    }

    vk::DescriptorSet VulkanDescriptorPool::allocate(const VulkanDescriptorSetLayout& descriptorSetLayout) {
        auto descriptorLayout = descriptorSetLayout.getDescriptorSetLayout();

        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo()
                .setDescriptorPool(mDescriptorPool)
                .setDescriptorSetCount(1)
                .setSetLayouts(descriptorLayout);

        vk::DescriptorSet descriptorSet = mLogicalDevice.allocateDescriptorSets(descriptorSetAllocateInfo)[0];
        return descriptorSet;
    }

    std::vector<vk::DescriptorSet> VulkanDescriptorPool::allocate(uint32_t descriptorSetCount,
                                                                  const VulkanDescriptorSetLayout&
                                                                  descriptorSetLayout) {
        ETERNAL_ASSERT(descriptorSetCount <= mMaxSets, "Descriptor set count exceeds max sets");

        std::vector<vk::DescriptorSetLayout> descriptorLayouts(descriptorSetCount,
                                                               descriptorSetLayout.getDescriptorSetLayout());

        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo()
                .setDescriptorPool(mDescriptorPool)
                .setDescriptorSetCount(descriptorSetCount)
                .setSetLayouts(descriptorLayouts);

        auto descSets = mLogicalDevice.allocateDescriptorSets(descriptorSetAllocateInfo);

        ETERNAL_ASSERT(descSets.size() == descriptorSetCount, "Failed to allocate descriptor sets");
        return descSets;
    }

    void VulkanDescriptorPool::free(vk::DescriptorSet* descriptorSet) {
        mLogicalDevice.freeDescriptorSets(mDescriptorPool, 1, descriptorSet);
    }

    void VulkanDescriptorPool::free(const std::vector<vk::DescriptorSet>& descriptorSets) {
        mLogicalDevice.freeDescriptorSets(mDescriptorPool, descriptorSets.size(), descriptorSets.data());
    }
}
