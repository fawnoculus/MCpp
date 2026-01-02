#include <cmath>
#include <stdfloat>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include "../Common-Lib/Logging.h"
#include "Client.h"

using std::float32_t, std::float64_t;

std::shared_ptr<spdlog::logger> g_glfwLogger = nullptr;
void glfwError(const int a_errorCode, const char *a_description) {
    if (g_glfwLogger == nullptr) return;
    g_glfwLogger->error("{}: {}", a_errorCode, a_description);
}

std::shared_ptr<spdlog::logger> g_vulkanLogger = nullptr;
VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    const vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*
) {
    if (g_vulkanLogger == nullptr) return VK_FALSE;
    switch (severity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            g_vulkanLogger->debug("{}: {}", to_string(type), std::string(pCallbackData->pMessage));
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            g_vulkanLogger->warn("{}: {}", to_string(type), std::string(pCallbackData->pMessage));
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            g_vulkanLogger->error("{}: {}", to_string(type), std::string(pCallbackData->pMessage));
            break;
        default:
            break;
    }
    return VK_FALSE;
}

std::vector<const char *> getRequiredLayers() {
#ifdef NDEBUG
    return std::vector<const char *>{};
#else
    return std::vector{
        "VK_LAYER_KHRONOS_validation"
    };
#endif
}

std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
    extensions.push_back(vk::EXTDebugUtilsExtensionName);
#endif

    return extensions;
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
        glfwSetWindowTitle(m_glfwWindow, std::format("Mcpp @{}FPS", round(fps)).c_str());
    }

    return EXIT_SUCCESS;
}

void Client::initGLFW() const {
    m_logger->debug("Initializing GLFW");
    if (!glfwInit()) {
        m_logger->error("Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwSetErrorCallback(glfwError);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Client::initVulkan() {
    auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();
#ifndef NDEBUG
    std::string supportedLayers = "[";
    for (auto layerProperty: layerProperties) {
        supportedLayers.append(std::format("\n\t\"{}\": \"{}\",", std::string(layerProperty.layerName),
                                           std::string(layerProperty.description)));
    }
    supportedLayers.append("\n]");
    m_logger->debug("Supported Vulkan Layers: {}", supportedLayers);
#endif

    m_logger->debug("Checking Required Vulkan Layers");
    auto requiredLayers = getRequiredLayers();
    if (std::ranges::any_of(requiredLayers, [&layerProperties](auto requiredLayer) {
        if (std::ranges::none_of(layerProperties, [requiredLayer](auto layerProperty) {
            return strcmp(layerProperty.layerName, requiredLayer) == 0;
        })) {
            spdlog::error("Required Vulkan Layer {} ist not supported", std::string(requiredLayer));
            return true;
        }

        return false;
    })) {
        m_logger->error("One ore more required Vulkan Layers are not supported");
        throw std::runtime_error("One ore more required Vulkan Layers are not supported");
    }

    auto extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
#ifndef NDEBUG
    std::string supportedProperties = "[";
    for (auto [extensionName, specVersion]: extensionProperties) {
        supportedProperties.append(std::format("\n\t\"{}\"", std::string(extensionName)));
    }
    supportedProperties.append("\n]");
    m_logger->debug("Supported Vulkan Extensions: {}", supportedProperties);
#endif

    auto requiredExtensions = getRequiredExtensions();
    m_logger->debug("Checking Required Vulkan Extensions");
    if (std::ranges::any_of(requiredExtensions, [&extensionProperties](auto requiredExtension) {
        if (std::ranges::none_of(extensionProperties, [requiredExtension](auto extensionProperty) {
            return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        })) {
            spdlog::error("Vulkan Extension {} ist not supported", std::string(requiredExtension));
            return true;
        }
        return false;
    })) {
        m_logger->error("One ore more required Vulkan Layers are not supported");
        throw std::runtime_error("One ore more required Vulkan Layers are not supported");
    }

    m_logger->debug("Initializing Vulkan");
    m_vkAppInfo.pApplicationName = "Mcpp";
    m_vkAppInfo.applicationVersion = VK_MAKE_VERSION(MCPP_VERSION_MAJOR, MCPP_VERSION_MINOR, MCPP_VERSION_PATCH);
    m_vkAppInfo.pEngineName = "Mcpp";
    m_vkAppInfo.engineVersion = VK_MAKE_VERSION(MCPP_VERSION_MAJOR, MCPP_VERSION_MINOR, MCPP_VERSION_PATCH);
    m_vkAppInfo.apiVersion = vk::ApiVersion14;

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
        throw std::runtime_error("Failed to initialize Vulkan, Vulkan Error: " + std::string(e.what()));
    } catch (std::exception &e) {
        m_logger->error("Failed to initialize Vulkan, Exception: {}", e.what());
        throw std::runtime_error("Failed to initialize Vulkan, Exception: " + std::string(e.what()));
    }

#ifndef NDEBUG
    constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    constexpr vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
    );
    constexpr vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &vkDebugCallback
        };
    debugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
#endif
}

void Client::createWindow() {
    m_logger->debug("Creating Window");
    m_glfwWindow = glfwCreateWindow(854, 480, "Mcpp", nullptr, nullptr);
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

void Client::pickVkDevice() {
    const auto devices = m_vkInstance.enumeratePhysicalDevices();
    if (devices.empty()) {
        m_logger->error("Failed to find GPU with Vulkan support");
        throw std::runtime_error("Failed to find GPU with Vulkan support");
    }

    vk::raii::PhysicalDevice currentBest = nullptr;
    int64_t currentBestScore = -9'223'372'036'854'775'807;
    for (const auto &device: devices) {
        if (!isDeviceSuitable(device)) {
            continue;
        }

        if (const int64_t deviceScore = getVkDeviceScore(device); currentBestScore < deviceScore) {
            currentBest = device;
            currentBestScore = deviceScore;
        }
    }

    if (currentBest == nullptr) {
        m_logger->error("Failed to find Vulkan supporting GPU that meets the minimum requirements");
        throw std::runtime_error("Failed to find Vulkan supporting GPU that meets the minimum requirements");
    }

    m_vkPhysicalDevice = currentBest;
    m_vkDevice = vk::raii::Device(currentBest, vk::DeviceCreateInfo{});
}


bool Client::isDeviceSuitable(const vk::PhysicalDevice &a_device) const {
    VkBool32 result = VK_FALSE;

    if (a_device.getSurfaceSupportKHR(0, m_vkSurface, &result) != vk::Result::eSuccess) {
        return false;
    }

    return result == VK_TRUE && a_device.getFeatures().geometryShader;
}
