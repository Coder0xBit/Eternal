#include "VulkanSwapChain.h"
#include <core/graphics/vulkan/VulkanPlatform.h>

namespace Eternal {
	VulkanSwapChain::VulkanSwapChain(
		vk::Instance instance,
		vk::Device logicalDevice,
		vk::PhysicalDevice physicalDevice,
		vk::Queue queue,
		vk::SurfaceKHR surface,
		vk::Extent2D extent,
		uint32_t graphicsQueueFamilyIndex,
		uint32_t presentQueueFamilyIndex
	) : m_VkInstance(instance),
		m_LogicalDevice(logicalDevice),
		m_PhysicalDevice(physicalDevice),
		m_PresentQueue(queue),
		m_Surface(surface),
		m_FallBackExtent(extent),
		m_GraphicsQueueFamilyIndex(graphicsQueueFamilyIndex),
		m_PresentQueueFamilyIndex(presentQueueFamilyIndex) {
		create();
	}

	VulkanSwapChain::~VulkanSwapChain() {
		destroy();
	}

	vk::Result VulkanSwapChain::acquire(vk::Semaphore imageReadySemaphore, uint32_t* imageIndex) {
		vk::Result result = m_LogicalDevice.acquireNextImageKHR(m_SwapChain, std::numeric_limits<uint64_t>::max(), imageReadySemaphore, nullptr, imageIndex);
		m_ShouldRecreate = result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR;
		return result;
	}

	vk::Result VulkanSwapChain::present(vk::Semaphore renderFinishedSemaphore, uint32_t imageIndex) {
		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&renderFinishedSemaphore)
			.setSwapchainCount(1)
			.setPSwapchains(&m_SwapChain)
			.setPImageIndices(&imageIndex);

		vk::Result result = m_PresentQueue.presentKHR(&presentInfo);
		m_ShouldRecreate = result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR;
		return result;
	}

	void VulkanSwapChain::recreate() {
		m_ShouldRecreate = false;
		destroy();
		create();
	}

	void VulkanSwapChain::create() {
		vk::SurfaceCapabilitiesKHR capabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);

		vk::SurfaceFormatKHR format = selectSwapChainSurfaceFormat();

		vk::PresentModeKHR presentMode = selectSwapChainPresentMode();
		Eternal::Logger::Debug("Selected Present Mode {}", vk::to_string(presentMode));

		vk::Extent2D extent = selectSwapChainExtent(capabilities);
		Eternal::Logger::Info("Selected Extent: {}x{}", extent.width, extent.height);

		m_SwapChainDetails.capabilities = capabilities;
		m_SwapChainDetails.surfaceFormat = format;
		m_SwapChainDetails.presentMode = presentMode;
		m_SwapChainDetails.extent = extent;

		uint32_t swapChainImageCount = m_SwapChainDetails.capabilities.minImageCount;
		if (m_SwapChainDetails.capabilities.maxImageCount > 0 && swapChainImageCount > m_SwapChainDetails.capabilities.maxImageCount) {
			swapChainImageCount = m_SwapChainDetails.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR swapChainCreateInfo = vk::SwapchainCreateInfoKHR()
			.setSurface(m_Surface)
			.setMinImageCount(swapChainImageCount)
			.setImageFormat(m_SwapChainDetails.surfaceFormat.format)
			.setImageColorSpace(m_SwapChainDetails.surfaceFormat.colorSpace)
			.setImageExtent(m_SwapChainDetails.extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		std::vector<uint32_t> queueFamilyIndices = {
			m_GraphicsQueueFamilyIndex,
			m_PresentQueueFamilyIndex
		};

		if (m_GraphicsQueueFamilyIndex != m_PresentQueueFamilyIndex) {
			swapChainCreateInfo
				.setImageSharingMode(vk::SharingMode::eConcurrent)
				.setQueueFamilyIndexCount(2)
				.setQueueFamilyIndices(queueFamilyIndices);
		}
		else {
			swapChainCreateInfo
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}

		swapChainCreateInfo
			.setPreTransform(m_SwapChainDetails.capabilities.currentTransform)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(m_SwapChainDetails.presentMode)
			.setClipped(VK_TRUE)
			.setOldSwapchain(VK_NULL_HANDLE);

		m_SwapChain = m_LogicalDevice.createSwapchainKHR(swapChainCreateInfo);

		createImageViews();
		createDepthImageView();
		createRenderPass();
		createFrameBuffers();
	}

	void VulkanSwapChain::destroy() {
		m_ShouldRecreate = false;

		m_LogicalDevice.waitIdle();

		for (auto imageView : m_SwapChainImageViews) {
			m_LogicalDevice.destroyImageView(imageView);
		}
		m_SwapChainImages.clear();

		for (auto frameBuffer : m_SwapChainFrameBuffers) {
			m_LogicalDevice.destroyFramebuffer(frameBuffer);
		}
		m_SwapChainFrameBuffers.clear();

		if (m_DepthImageView) {
			m_LogicalDevice.destroyImageView(m_DepthImageView);
			m_DepthImageView = nullptr;
		}

		if (m_DepthImage) {
			m_LogicalDevice.destroyImage(m_DepthImage);
			m_DepthImage = nullptr;
		}

		if (m_DepthImageMemory) {
			m_LogicalDevice.freeMemory(m_DepthImageMemory);
			m_DepthImageMemory = nullptr;
		}

		if (m_RenderPass) {
			m_LogicalDevice.destroyRenderPass(m_RenderPass);
			m_RenderPass = nullptr;
		}

		if (m_SwapChain) {
			m_LogicalDevice.destroySwapchainKHR(m_SwapChain);
			m_SwapChain = nullptr;
		}
	}

	vk::SurfaceFormatKHR VulkanSwapChain::selectSwapChainSurfaceFormat() {
		std::vector<vk::SurfaceFormatKHR> availableFormats = m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface);
		for (const vk::SurfaceFormatKHR& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR VulkanSwapChain::selectSwapChainPresentMode() {
		std::vector<vk::PresentModeKHR> availablePresentModes = m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface);
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D VulkanSwapChain::selectSwapChainExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			return m_FallBackExtent;
		}
	}

	void VulkanSwapChain::createImageViews() {
		m_SwapChainImages = m_LogicalDevice.getSwapchainImagesKHR(m_SwapChain);

		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (uint32_t i = 0; i < m_SwapChainImages.size(); i++) {
			vk::ImageSubresourceRange imageSubResourceRange = vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1);

			vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
				.setImage(m_SwapChainImages[i])
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(m_SwapChainDetails.surfaceFormat.format)
				.setComponents(vk::ComponentMapping())
				.setSubresourceRange(imageSubResourceRange);

			m_SwapChainImageViews[i] = m_LogicalDevice.createImageView(imageViewCreateInfo);
		}
	}

	void VulkanSwapChain::createDepthImageView() {
		vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setFormat(vk::Format::eD32Sfloat)
			.setExtent({ m_SwapChainDetails.extent.width, m_SwapChainDetails.extent.height, 1 })
			.setMipLevels(1)
			.setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

		m_DepthImage = m_LogicalDevice.createImage(imageCreateInfo);

		vk::MemoryRequirements memRequirements = m_LogicalDevice.getImageMemoryRequirements(m_DepthImage);

		uint32_t memoryTypeIndex = VulkanPlatform::getMemoryType(
			m_PhysicalDevice,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			memRequirements.memoryTypeBits
		);

		ETERNAL_ASSERT(memoryTypeIndex != 0xFFFFFFFF, "Failed to find suitable memory type for depth image");

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(memoryTypeIndex);

		m_DepthImageMemory = m_LogicalDevice.allocateMemory(allocInfo);

		m_LogicalDevice.bindImageMemory(m_DepthImage, m_DepthImageMemory, 0);

		vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setImage(m_DepthImage)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(vk::Format::eD32Sfloat)
			.setSubresourceRange({ vk::ImageAspectFlagBits::eDepth,0, 1, 0, 1 });

		m_DepthImageView = m_LogicalDevice.createImageView(imageViewCreateInfo);
	}

	void VulkanSwapChain::createRenderPass() {
		vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
			.setFormat(m_SwapChainDetails.surfaceFormat.format)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		vk::AttachmentReference colorAttachmentReference = vk::AttachmentReference()
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::AttachmentDescription depthAttachment = vk::AttachmentDescription()
			.setFormat(vk::Format::eD32Sfloat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::AttachmentReference depthAttachmentReference = vk::AttachmentReference()
			.setAttachment(1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::SubpassDescription subPass = vk::SubpassDescription()
			.setColorAttachmentCount(1)
			.setPColorAttachments(&colorAttachmentReference)
			.setPDepthStencilAttachment(&depthAttachmentReference);

		std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo()
			.setAttachments(attachments)
			.setSubpassCount(1)
			.setPSubpasses(&subPass);

		m_RenderPass = m_LogicalDevice.createRenderPass(renderPassCreateInfo);
	}

	void VulkanSwapChain::createFrameBuffers() {
		m_SwapChainFrameBuffers.resize(m_SwapChainImageViews.size());

		for (uint32_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			vk::ImageView attachment[] = { m_SwapChainImageViews[i] , m_DepthImageView };

			vk::FramebufferCreateInfo frameBufferInfo = vk::FramebufferCreateInfo()
				.setRenderPass(m_RenderPass)
				.setAttachments(attachment)
				.setWidth(m_SwapChainDetails.extent.width)
				.setHeight(m_SwapChainDetails.extent.height)
				.setLayers(1);

			m_SwapChainFrameBuffers[i] = m_LogicalDevice.createFramebuffer(frameBufferInfo);
		}
	}
}