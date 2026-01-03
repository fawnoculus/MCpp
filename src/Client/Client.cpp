#include <cmath>
#include <stdfloat>

#include <vulkan/vulkan_raii.hpp>
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include "../Common-Lib/Logging.h"
#include "Client.h"

using std::string, std::vector, std::shared_ptr, std::float64_t;

constexpr auto g_appName = "Mcpp";
constexpr auto g_appVkVersion = VK_MAKE_VERSION(MCPP_VERSION_MAJOR, MCPP_VERSION_MINOR, MCPP_VERSION_PATCH);
constexpr auto g_requiredVulkanVersion = vk::ApiVersion13;
shared_ptr<spdlog::logger> g_glfwLogger = nullptr;
shared_ptr<spdlog::logger> g_vulkanLogger = nullptr;

void glfwError(const int a_errorCode, const char *a_description) {
    if (g_glfwLogger == nullptr) return;
    g_glfwLogger->error("{}: {}", a_errorCode, a_description);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT a_severity,
    const vk::DebugUtilsMessageTypeFlagsEXT a_type,
    const vk::DebugUtilsMessengerCallbackDataEXT *a_pCallbackData, void *
) {
    if (g_vulkanLogger == nullptr) return VK_FALSE;
    switch (a_severity) {
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

Client::Client() {
    Logging::setupLogging();

    m_logger = Logging::getLogger("Client");
    m_logger->info("Initializing Client ...");

    g_glfwLogger = Logging::getLogger("GLFW");
    g_vulkanLogger = Logging::getLogger("Vulkan");

    initGLFW();
    initVulkan();
    createWindow();
    pickVkDevice();

    m_logger->info("Finished Client initialisation");
}

Client::~Client() {
    m_logger->info("Stoping Client ...");

    m_logger->debug("Destroying Window");
    glfwDestroyWindow(m_glfwWindow);

    m_logger->debug("Terminating GLFW");
    glfwTerminate();

    m_logger->flush();
}

int Client::run() const {
    m_logger->info("Running Client ...");

    while (!glfwWindowShouldClose(m_glfwWindow)) {
        std::chrono::time_point<std::chrono::system_clock> frameStart = std::chrono::high_resolution_clock::now();

        glfwPollEvents();

        std::chrono::time_point<std::chrono::system_clock> frameEnd = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float64_t> elapsed_seconds = frameEnd - frameStart;
        const float64_t fps = 1.f / elapsed_seconds.count();
        glfwSetWindowTitle(m_glfwWindow, std::format("{} @{}FPS", g_appName, round(fps)).c_str());
    }

    return EXIT_SUCCESS;
}

void Client::initGLFW() const {
    m_logger->debug("Initializing GLFW");
    if (glfwInit() != GLFW_TRUE) {
        m_logger->error("Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(glfwError);

    if (glfwVulkanSupported() != GLFW_TRUE) {
        m_logger->error("Current GLFW Doesn't support Vulkan");
        throw std::runtime_error("Current GLFW Doesn't support Vulkan");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

vector<const char *> getRequiredLayers() {
#ifdef NDEBUG
    return vector<const char *>{};
#else
    return vector{
        "VK_LAYER_KHRONOS_validation"
    };
#endif
}

vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
    extensions.push_back(vk::EXTDebugUtilsExtensionName);
#endif

    return extensions;
}

void Client::initVulkan() {
    auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();

    string supportedLayers = "[";
    for (auto layerProperty: layerProperties) {
        supportedLayers.append(std::format(R"("{}": "{}", )", string(layerProperty.layerName),
                                           string(layerProperty.description)));
    }
    supportedLayers.append("]");
    m_logger->debug("Supported Vulkan Layers: {}", supportedLayers);

    m_logger->debug("Checking Required Vulkan Layers");
    auto requiredLayers = getRequiredLayers();
    if (std::ranges::any_of(requiredLayers, [&layerProperties](auto requiredLayer) {
        if (std::ranges::none_of(layerProperties, [requiredLayer](auto layerProperty) {
            return strcmp(layerProperty.layerName, requiredLayer) == 0;
        })) {
            spdlog::error("Required Vulkan Layer {} ist not supported", string(requiredLayer));
            return true;
        }

        return false;
    })) {
        m_logger->error("One ore more required Vulkan Layers are not supported");
        throw std::runtime_error("One ore more required Vulkan Layers are not supported");
    }

    auto extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();

    string supportedProperties = "[";
    for (auto [extensionName, specVersion]: extensionProperties) {
        supportedProperties.append(std::format(R"("{}", )", string(extensionName)));
    }
    supportedProperties.append("]");
    m_logger->debug("Supported Vulkan Extensions: {}", supportedProperties);

    auto requiredExtensions = getRequiredExtensions();
    m_logger->debug("Checking Required Vulkan Extensions");
    if (std::ranges::any_of(requiredExtensions, [&extensionProperties](auto requiredExtension) {
        if (std::ranges::none_of(extensionProperties, [requiredExtension](auto extensionProperty) {
            return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        })) {
            spdlog::error("Vulkan Extension {} ist not supported", string(requiredExtension));
            return true;
        }
        return false;
    })) {
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

    try {
        m_vkInstance = vk::raii::Instance(m_vkContext, createInfo);
    } catch (vk::Error &e) {
        m_logger->error("Failed to initialize Vulkan, Vulkan Error: {}", e.what());
        throw std::runtime_error("Failed to initialize Vulkan, Vulkan Error: " + string(e.what()));
    } catch (std::exception &e) {
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
    m_debugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void Client::createWindow() {
    m_logger->debug("Creating Window");
    m_glfwWindow = glfwCreateWindow(854, 480, g_appName, nullptr, nullptr);
    if (!m_glfwWindow) {
        m_logger->error("Failed to create Window");
        throw std::runtime_error("Failed to create Window");
    }
    glfwShowWindow(m_glfwWindow);

    m_logger->debug("Creating Vulkan Window surface");
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*m_vkInstance, m_glfwWindow, nullptr, &surface) != GLFW_NO_ERROR) {
        m_logger->error("Failed to create Window surface");
        throw std::runtime_error("Failed to create Window Surface");
    }
    m_vkSurface = vk::raii::SurfaceKHR(m_vkInstance, surface);
}


int64_t getVkDeviceScore(const vk::PhysicalDevice &a_device) {
    const auto deviceProperties = a_device.getProperties();

    int64_t score = 0;

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D * deviceProperties.limits.maxImageArrayLayers;

    return score;
}

vector<const char *> getRequiredDeviceExtensions() {
    return {
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName
    };
}

void Client::pickVkDevice() {
    m_logger->debug("Picking Best Physical Vulkan device");
    const auto devices = m_vkInstance.enumeratePhysicalDevices();
    if (devices.empty()) {
        m_logger->error("Failed to find GPU with Vulkan support");
        throw std::runtime_error("Failed to find GPU with Vulkan support");
    }

    m_logger->debug("Found {} Vulkan device(s)", devices.size());
    const auto requiredDeviceExtensions = getRequiredDeviceExtensions();

    vk::raii::PhysicalDevice currentBest = nullptr;
    int64_t currentBestScore = -9'223'372'036'854'775'807;
    uint32_t graphicsQueueFamilyIndex = -1;
    uint32_t presentQueueFamilyIndex = -1;
    for (const auto &device: devices) {
        auto result = isDeviceSuitable(device, requiredDeviceExtensions);
        if (!result.isSuitable) {
            continue;
        }

        if (const int64_t deviceScore = getVkDeviceScore(device); currentBestScore < deviceScore) {
            currentBest = device;
            currentBestScore = deviceScore;
            graphicsQueueFamilyIndex = static_cast<uint32_t>(result.graphicsQueueFamilyIndex);
            presentQueueFamilyIndex = static_cast<uint32_t>(result.presentQueueFamilyIndex);
        }
    }

    if (currentBest == nullptr) {
        m_logger->error("Failed to find Physical Vulkan device that meets the minimum requirements");
        throw std::runtime_error("Failed to find Physical Vulkan device that meets the minimum requirements");
    }

    m_logger->debug("Picked Physical Vulkan device: {}", string(currentBest.getProperties().deviceName));
    m_vkPhysicalDevice = currentBest;

    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
        .queueFamilyIndex = graphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    //vk::PhysicalDeviceFeatures deviceFeatures;

    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
        {},
        {.dynamicRendering = true },
        {.extendedDynamicState = true }
    };

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredDeviceExtensions.data()
    };

    m_vkDevice = vk::raii::Device(currentBest, deviceCreateInfo);
    m_vkGraphicsQueue = vk::raii::Queue( m_vkDevice, graphicsQueueFamilyIndex, 0 );
    m_vkPresentQueue = vk::raii::Queue( m_vkDevice, presentQueueFamilyIndex, 0 );
}

Client::CheckDeviceResult Client::isDeviceSuitable(const vk::PhysicalDevice &a_device, const vector<const char *> &a_requiredDeviceExtensions) const {
    if (a_device.getProperties().apiVersion < g_requiredVulkanVersion) {
        return {};
    }

    if (!a_device.getFeatures().geometryShader) {
        return {};
    }

    const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = a_device.getQueueFamilyProperties();
    size_t graphicsQueueFamilyIndex = -1;
    size_t presentQueueFamilyIndex = -1;
    for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphicsQueueFamilyIndex = i;
        }
        if (VkBool32 result = VK_FALSE; a_device.getSurfaceSupportKHR(i, m_vkSurface, &result) == vk::Result::eSuccess && result == VK_TRUE) {
            presentQueueFamilyIndex = i;
        }
        if (graphicsQueueFamilyIndex == presentQueueFamilyIndex) {
            break;
        }
    }
    if (graphicsQueueFamilyIndex < 0
        || graphicsQueueFamilyIndex >= queueFamilyProperties.size()
        || presentQueueFamilyIndex < 0
        || presentQueueFamilyIndex >= queueFamilyProperties.size()
    ) {
        return {
            .graphicsQueueFamilyIndex = graphicsQueueFamilyIndex,
            .presentQueueFamilyIndex = presentQueueFamilyIndex,
        };
    }

    auto supportedDeviceExtensions = a_device.enumerateDeviceExtensionProperties();
    const bool areAllExtensionsSupported = !std::ranges::any_of(a_requiredDeviceExtensions, [&supportedDeviceExtensions](auto requiredExtension) {
        return std::ranges::none_of(supportedDeviceExtensions, [requiredExtension](auto extensionProperty) {
            return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        });
    });

    return {
        .isSuitable = areAllExtensionsSupported,
        .graphicsQueueFamilyIndex = graphicsQueueFamilyIndex,
        .presentQueueFamilyIndex = presentQueueFamilyIndex,
    };
}
