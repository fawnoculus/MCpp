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
    vk::ApplicationInfo m_vkAppInfo;
    vk::raii::Context m_vkContext;
    vk::raii::Instance m_vkInstance = nullptr;
    vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr;
    vk::raii::Device m_vkDevice = nullptr;
    vk::raii::SurfaceKHR m_vkSurface = nullptr;
    GLFWwindow *m_glfwWindow = nullptr;
#ifndef NDEBUG
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
#endif

    void initGLFW() const;
    void initVulkan();
    void createWindow();
    void pickVkDevice();
    [[nodiscard]]
    bool isDeviceSuitable(const vk::PhysicalDevice &a_device) const;
};
