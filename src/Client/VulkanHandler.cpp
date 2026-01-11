#include "spdlog/spdlog.h"
#include "vulkan/vulkan_raii.hpp"
#include "GLFW/glfw3.h"

#include "../Common-Lib/Logging.h"
#include "VulkanHandler.h"

using std::vector, std::string, std::shared_ptr, std::optional;

constexpr auto g_appName = "Mcpp";
constexpr uint32_t g_appVkVersion = VK_MAKE_VERSION(MCPP_VERSION_MAJOR, MCPP_VERSION_MINOR, MCPP_VERSION_PATCH);
constexpr uint32_t g_requiredVulkanVersion = vk::ApiVersion13;
shared_ptr<spdlog::logger> g_vulkanLogger = nullptr;

VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT a_severity,
    const vk::DebugUtilsMessageTypeFlagsEXT a_type,
    const vk::DebugUtilsMessengerCallbackDataEXT *a_pCallbackData, void *
)
{
    if (g_vulkanLogger == nullptr) return VK_FALSE;
    switch (a_severity)
    {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            g_vulkanLogger->warn("{}: {}", to_string(a_type), string(a_pCallbackData->pMessage));
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            g_vulkanLogger->error("{}: {}", to_string(a_type), string(a_pCallbackData->pMessage));
            break;
        default:
            break;
    }
    return VK_FALSE;
}

VulkanHandler::VulkanHandler(const std::shared_ptr<spdlog::logger> &a_logger)
    : m_logger(a_logger)
{
    g_vulkanLogger = Logging::getLogger("Vulkan");
}

vector<const char *> getRequiredLayers()
{
#ifdef NDEBUG
    return vector<const char *>{};
#else
    return vector{
        "VK_LAYER_KHRONOS_validation"
    };
#endif
}

vector<const char *> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
    extensions.push_back(vk::EXTDebugUtilsExtensionName);
#endif

    return extensions;
}

void VulkanHandler::initialize()
{
    auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();

    string supportedLayers = "[";
    for (auto layerProperty: layerProperties)
    {
        supportedLayers.append(std::format("\"{}\": \"{}\", ", string(layerProperty.layerName),
                                           string(layerProperty.description)));
    }
    supportedLayers.append("]");
    m_logger->debug("Supported Vulkan Layers: {}", supportedLayers);

    m_logger->debug("Checking Required Vulkan Layers");
    auto requiredLayers = getRequiredLayers();
    if (std::ranges::any_of(requiredLayers, [&layerProperties](auto requiredLayer)
    {
        if (std::ranges::none_of(layerProperties, [requiredLayer](auto layerProperty)
        {
            return strcmp(layerProperty.layerName, requiredLayer) == 0;
        }))
        {
            spdlog::error("Required Vulkan Layer {} ist not supported", string(requiredLayer));
            return true;
        }

        return false;
    }))
    {
        m_logger->error("One ore more required Vulkan Layers are not supported");
        throw std::runtime_error("One ore more required Vulkan Layers are not supported");
    }

    auto extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();

    string supportedProperties = "[";
    for (auto [extensionName, specVersion]: extensionProperties)
    {
        supportedProperties.append(std::format("\"{}\", ", string(extensionName)));
    }
    supportedProperties.append("]");
    m_logger->debug("Supported Vulkan Extensions: {}", supportedProperties);

    auto requiredExtensions = getRequiredExtensions();
    m_logger->debug("Checking Required Vulkan Extensions");
    if (std::ranges::any_of(requiredExtensions, [&extensionProperties](auto requiredExtension)
    {
        if (std::ranges::none_of(extensionProperties, [requiredExtension](auto extensionProperty)
        {
            return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        }))
        {
            spdlog::error("Vulkan Extension {} ist not supported", string(requiredExtension));
            return true;
        }
        return false;
    }))
    {
        m_logger->error("One ore more required Vulkan Layers are not supported");
        throw std::runtime_error("One ore more required Vulkan Layers are not supported");
    }

    m_logger->debug("Initializing Vulkan");
    m_vkAppInfo.pApplicationName = g_appName;
    m_vkAppInfo.applicationVersion = g_appVkVersion;
    m_vkAppInfo.pEngineName = g_appName;
    m_vkAppInfo.engineVersion = g_appVkVersion;
    m_vkAppInfo.apiVersion = g_requiredVulkanVersion;

    const vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &m_vkAppInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };

    try
    {
        m_vkInstance = vk::raii::Instance(m_vkContext, createInfo);
    } catch (vk::Error &e)
    {
        m_logger->error("Failed to initialize Vulkan, Vulkan Error: {}", e.what());
        throw std::runtime_error("Failed to initialize Vulkan, Vulkan Error: " + string(e.what()));
    } catch (std::exception &e)
    {
        m_logger->error("Failed to initialize Vulkan, Exception: {}", e.what());
        throw std::runtime_error("Failed to initialize Vulkan, Exception: " + string(e.what()));
    }

    constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    constexpr vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
    );
    constexpr vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &vkDebugCallback
    };
    m_vkDebugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void VulkanHandler::setWindow(GLFWwindow *a_glfwWindow)
{
    m_glfwWindow = a_glfwWindow;

    m_logger->debug("Creating Vulkan Window surface");
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*m_vkInstance, m_glfwWindow, nullptr, &surface) != GLFW_NO_ERROR)
    {
        m_logger->error("Failed to create Window surface");
        throw std::runtime_error("Failed to create Window Surface");
    }
    m_vkSurface.clear();
    m_vkSurface = vk::raii::SurfaceKHR(m_vkInstance, surface);

    pickVkDevice();
    createVkSwapChain();
    createVkImageViews();
}

void VulkanHandler::setResourceManager(ResourceManager *a_resourceManager)
{
    m_resourceManager = a_resourceManager;
    m_resourceManager->setVkDevice(m_vkDevice);
}

void VulkanHandler::onWindowResize(const GLFWwindow *a_glfwWindow, int a_width, int a_height)
{
    createVkSwapChain();
    createVkImageViews();
}

int64_t getVkDeviceScore(const vk::PhysicalDevice &a_device)
{
    const auto deviceProperties = a_device.getProperties();

    int64_t score = 0;

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D * deviceProperties.limits.maxImageArrayLayers;

    return score;
}

vector<const char *> getRequiredDeviceExtensions()
{
    return {
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName
    };
}

void VulkanHandler::pickVkDevice()
{
    m_logger->debug("Picking Best Physical Vulkan device");
    const auto devices = m_vkInstance.enumeratePhysicalDevices();
    if (devices.empty())
    {
        m_logger->error("Failed to find GPU with Vulkan support");
        throw std::runtime_error("Failed to find GPU with Vulkan support");
    }

    m_logger->debug("Found {} Vulkan device(s)", devices.size());
    const auto requiredDeviceExtensions = getRequiredDeviceExtensions();

    vk::raii::PhysicalDevice currentBest = nullptr;
    int64_t currentBestScore = -9'223'372'036'854'775'807;
    CheckDeviceResult currentBestCheckResult = {};
    for (const auto &device: devices)
    {
        CheckDeviceResult result = checkVkDevice(device, requiredDeviceExtensions);
        if (!result.isSuitable)
        {
            continue;
        }

        if (const int64_t deviceScore = getVkDeviceScore(device); currentBestScore < deviceScore)
        {
            currentBest = device;
            currentBestScore = deviceScore;
            currentBestCheckResult = result;
        }
    }

    if (currentBest == nullptr)
    {
        m_logger->error("Failed to find Physical Vulkan device that meets the minimum requirements");
        throw std::runtime_error("Failed to find Physical Vulkan device that meets the minimum requirements");
    }

    m_logger->debug("Picked Physical Vulkan device: {}", string(currentBest.getProperties().deviceName));
    m_vkPhysicalDevice.clear();
    m_vkPhysicalDevice = currentBest;

    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = static_cast<uint32_t>(currentBestCheckResult.graphicsQueueFamilyIndex),
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    //vk::PhysicalDeviceFeatures deviceFeatures;

    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
        {},
        {.dynamicRendering = true},
        {.extendedDynamicState = true}
    };

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredDeviceExtensions.data()
    };

    m_vkDevice.clear();
    m_vkDevice = vk::raii::Device(currentBest, deviceCreateInfo);

    m_vkGraphicsQueueFamilyIndex = currentBestCheckResult.graphicsQueueFamilyIndex;
    m_vkGraphicsQueue.clear();
    m_vkGraphicsQueue = vk::raii::Queue(m_vkDevice, static_cast<uint32_t>(currentBestCheckResult.graphicsQueueFamilyIndex), 0);

    m_vkPresentQueueFamilyIndex = currentBestCheckResult.presentQueueFamilyIndex;
    m_vkPresentQueue.clear();
    m_vkPresentQueue = vk::raii::Queue(m_vkDevice, static_cast<uint32_t>(currentBestCheckResult.presentQueueFamilyIndex), 0);

    m_vkSurfaceFormat = currentBestCheckResult.surfaceFormat.value();
    m_vkSwapChainImageFormat = m_vkSurfaceFormat.format;

    if (m_resourceManager != nullptr)
    {
        m_resourceManager->setVkDevice(m_vkDevice);
    }

    for (GraphicsPipeline *pipeline : m_graphicsPipelines)
    {
        if (pipeline != nullptr)
        {
            pipeline->setVkDevice(m_vkDevice);
        }
    }
}

optional<vk::SurfaceFormatKHR> chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &a_availableFormats)
{
    for (const auto &availableFormat: a_availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return {};
}

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &a_availablePresentModes)
{
    for (const auto &availablePresentMode: a_availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &a_capabilities, GLFWwindow *a_glfwWindow)
{
    if (a_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return a_capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(a_glfwWindow, &width, &height);

    return {
        std::clamp<uint32_t>(width, a_capabilities.minImageExtent.width, a_capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, a_capabilities.minImageExtent.height, a_capabilities.maxImageExtent.height)
    };
}

VulkanHandler::CheckDeviceResult VulkanHandler::checkVkDevice(const vk::PhysicalDevice &a_device, const vector<const char *> &a_requiredDeviceExtensions) const
{
    if (a_device.getProperties().apiVersion < g_requiredVulkanVersion)
    {
        return {};
    }

    if (!a_device.getFeatures().geometryShader)
    {
        return {};
    }

    const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = a_device.getQueueFamilyProperties();
    auto graphicsQueueFamilyIndex = static_cast<size_t>(-1);
    auto presentQueueFamilyIndex = static_cast<size_t>(-1);
    for (size_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            graphicsQueueFamilyIndex = i;
        }
        if (VkBool32 result = VK_FALSE; a_device.getSurfaceSupportKHR(i, m_vkSurface, &result) == vk::Result::eSuccess && result == VK_TRUE)
        {
            presentQueueFamilyIndex = i;
        }
        if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
        {
            break;
        }
    }
    if (graphicsQueueFamilyIndex < 0
        || graphicsQueueFamilyIndex >= queueFamilyProperties.size()
        || presentQueueFamilyIndex < 0
        || presentQueueFamilyIndex >= queueFamilyProperties.size()
    )
    {
        return {
            .graphicsQueueFamilyIndex = graphicsQueueFamilyIndex,
            .presentQueueFamilyIndex = presentQueueFamilyIndex,
        };
    }

    if (auto supportedDeviceExtensions = a_device.enumerateDeviceExtensionProperties();
        std::ranges::any_of(a_requiredDeviceExtensions, [&supportedDeviceExtensions](auto requiredExtension)
        {
            return std::ranges::none_of(supportedDeviceExtensions, [requiredExtension](auto extensionProperty)
            {
                return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
            });
        }))
    {
        return {
            .graphicsQueueFamilyIndex = graphicsQueueFamilyIndex,
            .presentQueueFamilyIndex = presentQueueFamilyIndex,
        };
    }

    const auto surfaceFormat = chooseSwapSurfaceFormat(a_device.getSurfaceFormatsKHR(m_vkSurface));
    if (!surfaceFormat.has_value())
    {
        return {
            .graphicsQueueFamilyIndex = graphicsQueueFamilyIndex,
            .presentQueueFamilyIndex = presentQueueFamilyIndex,
            .surfaceFormat = surfaceFormat,
        };
    }

    return {
        .graphicsQueueFamilyIndex = graphicsQueueFamilyIndex,
        .presentQueueFamilyIndex = presentQueueFamilyIndex,
        .isSuitable = true,
        .surfaceFormat = surfaceFormat,
    };
}

void VulkanHandler::createVkSwapChain()
{
    m_logger->debug("Creating Vulkan SwapChain");

    const auto surfaceCapabilities = m_vkPhysicalDevice.getSurfaceCapabilitiesKHR(m_vkSurface);
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
    {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    m_vkSwapExtent = chooseSwapExtent(surfaceCapabilities, m_glfwWindow);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .flags = vk::SwapchainCreateFlagsKHR(),
        .surface = m_vkSurface,
        .minImageCount = minImageCount,
        .imageFormat = m_vkSurfaceFormat.format,
        .imageColorSpace = m_vkSurfaceFormat.colorSpace,
        .imageExtent = m_vkSwapExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = chooseSwapPresentMode(m_vkPhysicalDevice.getSurfacePresentModesKHR(m_vkSurface)),
        .clipped = vk::True,
        .oldSwapchain = VK_NULL_HANDLE
    };

    if (m_vkGraphicsQueueFamilyIndex != m_vkPresentQueueFamilyIndex)
    {
        const uint32_t queueFamilyIndices[] = {m_vkGraphicsQueueFamilyIndex, m_vkPresentQueueFamilyIndex};
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    m_vkSwapChain.clear();
    m_vkSwapChain = vk::raii::SwapchainKHR(m_vkDevice, swapChainCreateInfo);
    m_vkSwapChainImages = m_vkSwapChain.getImages();
}

void VulkanHandler::createVkImageViews()
{
    m_logger->debug("Creating Vulkan Image Views from SwapChain Images");
    m_vkSwapChainImageViews.clear();

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = m_vkSwapChainImageFormat,
        .components = {
            .r = vk::ComponentSwizzle::eIdentity,
            .g = vk::ComponentSwizzle::eIdentity,
            .b = vk::ComponentSwizzle::eIdentity,
            .a = vk::ComponentSwizzle::eIdentity,
        },
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
    };

    for (const auto image: m_vkSwapChainImages)
    {
        imageViewCreateInfo.image = image;
        m_vkSwapChainImageViews.emplace_back(m_vkDevice, imageViewCreateInfo);
    }
}
