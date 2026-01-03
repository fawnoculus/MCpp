#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include "vulkan/vulkan_raii.hpp"
#include "GLFW/glfw3.h"

class Client final {
public:
    Client();

    ~Client();

    [[nodiscard]]
    int run() const;

private:
    std::shared_ptr<spdlog::logger> m_logger;
    GLFWwindow *m_glfwWindow = nullptr;
    vk::ApplicationInfo m_vkAppInfo;
    vk::raii::Context m_vkContext;
    vk::raii::Instance m_vkInstance = nullptr;
    vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr;
    uint32_t m_vkQueueFamilyIndex = -1;
    vk::raii::Device m_vkDevice = nullptr;
    vk::raii::Queue m_vkGraphicsQueue = nullptr;
    vk::raii::Queue m_vkPresentQueue = nullptr;
    vk::raii::SurfaceKHR m_vkSurface = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

    void initGLFW() const;

    void initVulkan();

    void createWindow();

    void pickVkDevice();

    struct CheckDeviceResult {
        bool isSuitable = false;
        size_t graphicsQueueFamilyIndex = -1;
        size_t presentQueueFamilyIndex = -1;
    };

    [[nodiscard]]
    CheckDeviceResult isDeviceSuitable(const vk::PhysicalDevice &a_device, const std::vector<const char *> &a_requiredDeviceExtensions) const;
};
