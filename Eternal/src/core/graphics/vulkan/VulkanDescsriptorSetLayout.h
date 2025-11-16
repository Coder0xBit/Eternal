#pragma once

#include <core/graphics/vulkan/VulkanUtils.h>
#include <utils/Base.h>

namespace Eternal {
    class VulkanDescriptorSetLayout {
    public:
        using VulkanDescriptorBinding = std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>;

        struct LayoutInfo {
            uint32_t binding;
            vk::DescriptorType type;
            vk::ShaderStageFlags stageFlags;
            uint32_t count = 1;
        };

        struct BuilderDetails {
            vk::Device logicalDevice;
            VulkanDescriptorBinding bindings;
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
            Builder& addBinding(LayoutInfo layoutInfo);
            VulkanDescriptorSetLayout* build() noexcept;
        };

        VulkanDescriptorSetLayout(const Builder& builder);
        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
        VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;
        const vk::DescriptorSetLayout& getDescriptorSetLayout() const { return m_DescriptorSetLayout; }
        const VulkanDescriptorBinding& getBindings() const { return m_Binding; }
        ~VulkanDescriptorSetLayout();

    private:
        vk::Device m_LogicalDevice;
        VulkanDescriptorBinding m_Binding;
        vk::DescriptorSetLayout m_DescriptorSetLayout;
    };
}
