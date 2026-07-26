#include "core/graphics/vulkan/VulkanPlatform.h"
#include "core/graphics/vulkan/VulkanConstants.h"
#include "core/resource/ResourceManager.h"
#include "core/resource/ShaderProgram.h"

#include <set>
#include <GLFW/glfw3.h>

namespace Vortak {
    VulkanPlatform::VulkanPlatform(const Builder& builder) {
        mApplicationName = builder->applicationName;

        VulkanPlatform::initialize();
    }

    VulkanPlatform::~VulkanPlatform() {
        VulkanPlatform::shutDown();
    }

    void VulkanPlatform::initialize() {
        mVkInstance = createInstance(mApplicationName);
        mPhysicalDevice = choosePhysicalDevice(mVkInstance);
    }

    vk::Instance VulkanPlatform::createInstance(const std::string& applicationName) {
        uint32_t version = 0;
        vkEnumerateInstanceVersion(&mVersion);

        Vortak::Logger::Info("Vulkan Variant {}", VK_API_VERSION_VARIANT(mVersion));
        Vortak::Logger::Info("Vulkan Major {}", VK_API_VERSION_MAJOR(mVersion));
        Vortak::Logger::Info("Vulkan Minor {}", VK_API_VERSION_MINOR(mVersion));
        Vortak::Logger::Info("Vulkan Patch {}", VK_API_VERSION_PATCH(mVersion));

        // Removing Patch
        version = mVersion & ~(0xFFFU);

        vk::ApplicationInfo applicationInfo = vk::ApplicationInfo()
                                             .setPApplicationName(applicationName.c_str())
                                             .setPEngineName("Vortak")
                                             .setApiVersion(version)
                                             .setEngineVersion(version)
                                             .setApplicationVersion(version);

        uint32_t extensionCount = 0;
        VkStringArrayPtr glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        if (glfwExtensions == nullptr) {
            Vortak::Logger::Error(
                "vulkan glfwExtensions are null , probably before call glfwInit() function before createInstance()");
            return nullptr;
        }

        VkStringArray extensions(glfwExtensions, glfwExtensions + extensionCount);

        for (VkString extensionName : extensions)
            Vortak::Logger::Info("extension = {}", extensionName);

        VkStringArray layers;
#if VORTAK_FLAG_ENABLED(Vortak_VULKAN_DEBUG_VALIDATION)
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        if (!validateExtensions(extensions)) {
            Vortak::Logger::Error("validation failed for extensions");
            return nullptr;
        }

        if (!validateLayers(layers)) {
            Vortak::Logger::Error("validation failed for layer");
            return nullptr;
        }

        vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo()
                                           .setFlags(vk::InstanceCreateFlags())
                                           .setPApplicationInfo(&applicationInfo)
                                           .setPEnabledLayerNames(layers)
                                           .setPEnabledExtensionNames(extensions);

        return vk::createInstance(createInfo, nullptr);
    }

    vk::PhysicalDevice VulkanPlatform::choosePhysicalDevice(const vk::Instance& instance) {
        std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

        for (vk::PhysicalDevice& device : availableDevices) {
            if (checkDeviceIsSuitable(device) && device.getProperties().deviceType ==
                vk::PhysicalDeviceType::eDiscreteGpu) {
                Vortak::Logger::Info("************ Supported Device Properties ************");
                logDeviceProps(device);
                return device;
            }
        }

        return nullptr;
    }

    vk::Device VulkanPlatform::createLogicalDevice(vk::PhysicalDevice& device, uint32_t graphicsQueueFamilyIndex,
                                                   uint32_t presentQueueFamilyIndex) {
        vk::Device logicalDevice;
        float queuePriority = 1.0f;

        VkStringArray deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        std::vector<uint32_t> uniqueQueueFamilyIndices;
        uniqueQueueFamilyIndices.push_back(graphicsQueueFamilyIndex);
        if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
            uniqueQueueFamilyIndices.push_back(presentQueueFamilyIndex);
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
            vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
                                                       .setFlags(vk::DeviceQueueCreateFlags())
                                                       .setQueueFamilyIndex(queueFamilyIndex)
                                                       .setQueueCount(1)
                                                       .setPQueuePriorities(&queuePriority);

            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeature = vk::PhysicalDeviceFeatures();
        deviceFeature.setSamplerAnisotropy(true);

        vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
                                               .setFlags(vk::DeviceCreateFlags())
                                               .setPEnabledExtensionNames(deviceExtensions)
                                               .setQueueCreateInfos(queueCreateInfos)
                                               .setPEnabledFeatures(&deviceFeature);

        logicalDevice = device.createDevice(deviceCreateInfo);

        return logicalDevice;
    }

    void VulkanPlatform::shutDown() {
        if (mSurface) {
            mVkInstance.destroySurfaceKHR(mSurface);
        }

        mVkInstance.destroy();
    }

    SwapChain* VulkanPlatform::createSwapChain(Window* window) {
        mSurface = createWindowSurface(window);
        vk::Extent2D fallbackExtent = getExtent(window);

        mPresentQueueFamilyIndex = identifyPresentQueueFamilyIndex(mPhysicalDevice, mSurface);
        VORTAK_ASSERT(mPresentQueueFamilyIndex != INVALID_VK_INDEX, "Present Queue Family Index is Invalid");

        mGraphicsQueueFamilyIndex = identifyGraphicsQueueFamilyIndex(mPhysicalDevice, vk::QueueFlagBits::eGraphics);
        VORTAK_ASSERT(mGraphicsQueueFamilyIndex != INVALID_VK_INDEX, "Graphics Queue Family Index is Invalid");

        mLogicalDevice = createLogicalDevice(mPhysicalDevice, mGraphicsQueueFamilyIndex, mPresentQueueFamilyIndex);

        mPresentQueue = mLogicalDevice.getQueue(mPresentQueueFamilyIndex, mPresentQueueIndex);

        mGraphicsQueue = mLogicalDevice.getQueue(mGraphicsQueueFamilyIndex, mGraphicsQueueIndex);

        return Memory::Allocate<VulkanSwapChain>(mVkInstance, mLogicalDevice, mPhysicalDevice, mPresentQueue,
                                                 mSurface, fallbackExtent, mGraphicsQueueFamilyIndex,
                                                 mPresentQueueFamilyIndex);
    }

    vk::ShaderModule VulkanPlatform::loadShader(const std::filesystem::path& path) const {
        VORTAK_ASSERT(mLogicalDevice, "Logical Device not created, call createSwapChain(...) function");
        auto loadedShader = ResourceManager::get().load<ShaderProgram>(path.string());
        auto* shader = loadedShader.resource;
        const uint32_t* shaderCode = reinterpret_cast<uint32_t*>(shader->getBlob().data());

        vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
                                                           .setCodeSize(shader->getBlobSize())
                                                           .setPCode(shaderCode);

        vk::ShaderModule shaderModule = mLogicalDevice.createShaderModule(shaderModuleCreateInfo);
        return shaderModule;
    }

    uint32_t VulkanPlatform::identifyGraphicsQueueFamilyIndex(vk::PhysicalDevice& device, vk::QueueFlags flags) {
        uint32_t graphicsQueueFamilyIndex = INVALID_VK_INDEX;
        std::vector<vk::QueueFamilyProperties> queueFamiliesProperties = device.getQueueFamilyProperties();
        for (uint32_t i = 0; i < queueFamiliesProperties.size(); i++) {
            vk::QueueFamilyProperties props = queueFamiliesProperties[i];
            if (props.queueCount != 0 && props.queueFlags & flags) {
                graphicsQueueFamilyIndex = i;
                break;
            }
        }
        return graphicsQueueFamilyIndex;
    }

    uint32_t VulkanPlatform::identifyPresentQueueFamilyIndex(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
        uint32_t presentQueueFamilyIndex = INVALID_VK_INDEX;
        std::vector<vk::QueueFamilyProperties> queueFamiliesProperties = device.getQueueFamilyProperties();
        for (uint32_t i = 0; i < queueFamiliesProperties.size(); i++) {
            if (device.getSurfaceSupportKHR(i, surface)) {
                presentQueueFamilyIndex = i;
                break;
            }
        }
        return presentQueueFamilyIndex;
    }

    uint32_t VulkanPlatform::getMemoryType(vk::PhysicalDevice physicalDevice, vk::MemoryPropertyFlags properties,
                                           uint32_t typeBits) {
        vk::PhysicalDeviceMemoryProperties prop = physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
            if ((prop.memoryTypes[i].propertyFlags & properties) == properties && typeBits & (1 << i))
                return i;
        return 0xFFFFFFFF;
    }

    bool VulkanPlatform::validateExtensions(VkStringArray extensions) {
        std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

        for (VkString extension : extensions) {
            auto it = std::find_if(supportedExtensions.begin(), supportedExtensions.end(),
                                   [&](const vk::ExtensionProperties& supportedExtension) {
                                       return strcmp(extension, supportedExtension.extensionName) == 0;
                                   });

            if (it != supportedExtensions.end()) {
                Vortak::Logger::Debug("{} Extension Supported", extension);
            }
            else {
                Vortak::Logger::Error("{} Extension Not Supported", extension);
                return false;
            }
        }

        return true;
    }

    bool VulkanPlatform::validateLayers(VkStringArray layers) {
        std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

        for (VkString layer : layers) {
            auto it = std::find_if(supportedLayers.begin(), supportedLayers.end(),
                                   [&](const vk::LayerProperties& supportedLayer) {
                                       return strcmp(layer, supportedLayer.layerName) == 0;
                                   });

            if (it != supportedLayers.end()) {
                Vortak::Logger::Debug("{} Layer Supported", layer);
            }
            else {
                Vortak::Logger::Error("{} Layer Not Supported", layer);
                return false;
            }
        }

        return true;
    }

    bool VulkanPlatform::checkDeviceExtensionSupport(const vk::PhysicalDevice& device,
                                                     const VkStringArray requestedExtensions) {
        std::set<std::string> requiredExtesnions(requestedExtensions.begin(), requestedExtensions.end());

        for (vk::ExtensionProperties& extensionProperty : device.enumerateDeviceExtensionProperties()) {
            requiredExtesnions.erase(extensionProperty.extensionName);
        }

        return requiredExtesnions.empty();
    }

    bool VulkanPlatform::checkDeviceIsSuitable(const vk::PhysicalDevice& device) {
        VkStringArray requestedExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        bool isExtensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions);
        return isExtensionsSupported;
    }

    void VulkanPlatform::logDeviceProps(const vk::PhysicalDevice& device) {
        vk::PhysicalDeviceProperties properties = device.getProperties();

        Vortak::Logger::Debug("Device Name : {}", properties.deviceName.data());

        std::string deviceType;
        switch (properties.deviceType) {
            case (vk::PhysicalDeviceType::eCpu):
                deviceType = "CPU";
                break;

            case (vk::PhysicalDeviceType::eDiscreteGpu):
                deviceType = "Discrete GPU";
                break;

            case (vk::PhysicalDeviceType::eIntegratedGpu):
                deviceType = "Integrated GPU";
                break;

            case (vk::PhysicalDeviceType::eVirtualGpu):
                deviceType = "Virtual GPU";
                break;

            default:
                deviceType = "Other";
                break;
        }
        Vortak::Logger::Debug("Device Type : {}", deviceType);
    }

    vk::CommandPool VulkanPlatform::createCommandPool(vk::CommandPoolCreateFlags commandPoolCreateFlagBits) {
        vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
                                                         .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                                         .setQueueFamilyIndex(mGraphicsQueueIndex);

        return mLogicalDevice.createCommandPool(commandPoolCreateInfo);
    }

    std::vector<vk::CommandBuffer> VulkanPlatform::allocateCommandBuffers(
        vk::CommandPool commandPool, vk::CommandBufferLevel level, uint32_t count) {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
                                                                 .setCommandPool(commandPool)
                                                                 .setLevel(level)
                                                                 .setCommandBufferCount(count);

        return mLogicalDevice.allocateCommandBuffers(commandBufferAllocateInfo);
    }

    vk::CommandBuffer VulkanPlatform::allocateCommandBuffer(vk::CommandPool commandPool, vk::CommandBufferLevel level) {
        auto commandBuffer = allocateCommandBuffers(commandPool, level, 1);
        return commandBuffer.front();
    }

    void VulkanPlatform::destroyCommandPool(vk::CommandPool commandPool) {
        mLogicalDevice.destroyCommandPool(commandPool);
    }

    vk::CommandBuffer VulkanPlatform::beginSingleCommand(vk::CommandPool commandPool) {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
                                                                 .setCommandPool(commandPool)
                                                                 .setLevel(vk::CommandBufferLevel::ePrimary)
                                                                 .setCommandBufferCount(1);

        vk::CommandBuffer commandBuffer = mLogicalDevice.allocateCommandBuffers(commandBufferAllocateInfo)[0];

        vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
                                              .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
                                              .setPInheritanceInfo(nullptr);

        commandBuffer.begin(beginInfo);
        return commandBuffer;
    }

    void VulkanPlatform::endSingleCommand(vk::CommandPool commandPool, vk::CommandBuffer commandBuffer,
                                          vk::Queue queue) {
        commandBuffer.end();
        vk::SubmitInfo submitInfo = vk::SubmitInfo()
           .setCommandBuffers(commandBuffer);

        queue.submit(submitInfo, nullptr);
        queue.waitIdle();
        mLogicalDevice.freeCommandBuffers(commandPool, commandBuffer);
    }

    void VulkanPlatform::executeOneCommand(vk::CommandPool commandPool, vk::Queue queue,
                                           const std::function<void(vk::CommandBuffer)>& function) {
        vk::CommandBuffer commandBuffer = beginSingleCommand(commandPool);
        function(commandBuffer);
        endSingleCommand(commandPool, commandBuffer, queue);
    }

    vk::SurfaceKHR VulkanPlatform::createWindowSurface(Vortak::Window* window) const {
        VORTAK_ASSERT(window != nullptr, "Window is null");
        auto glfwWindow = static_cast<GLFWwindow*>(window->getNativeWindow());

        vk::SurfaceKHR surface = nullptr;
        if (glfwCreateWindowSurface(
                mVkInstance,
                glfwWindow,
                nullptr,
                reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS
        ) {
            Vortak::Logger::Error("Failed to create window surface");
            return nullptr;
        }
        return surface;
    }

    vk::Extent2D VulkanPlatform::getExtent(Vortak::Window* window) {
        VORTAK_ASSERT(window != nullptr, "Window is null");
        int width = 0, height = 0;
        auto glfwWindow = static_cast<GLFWwindow*>(window->getNativeWindow());
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }
}
