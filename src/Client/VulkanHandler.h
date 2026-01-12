#pragma once

#include "GraphicsPipeline.h"
#include "ResourceManager.h"
#include "spdlog/spdlog.h"
#include "vulkan/vulkan_raii.hpp"

class VulkanHandler final
{
public:
    VulkanHandler(std::nullptr_t) {}

    explicit VulkanHandler(const std::shared_ptr<spdlog::logger> &a_logger);

    void initialize();

    void setWindow(GLFWwindow *a_glfwWindow);

    void setResourceManager(ResourceManager *a_resourceManager);

    void onWindowResize(const GLFWwindow *a_glfwWindow, int a_width, int a_height);

private:
    std::shared_ptr<spdlog::logger> m_logger = nullptr;
    ResourceManager *m_resourceManager = nullptr;
    GLFWwindow *m_glfwWindow = nullptr;
    std::vector<GraphicsPipeline *> m_graphicsPipelines;
    vk::ApplicationInfo m_vkAppInfo;
    vk::raii::Context m_vkContext;
    vk::raii::Instance m_vkInstance = nullptr;
    vk::raii::SurfaceKHR m_vkSurface = nullptr;
    vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr;
    vk::raii::Device m_vkDevice = nullptr;
    uint32_t m_vkGraphicsQueueFamilyIndex = -1;
    uint32_t m_vkPresentQueueFamilyIndex = -1;
    vk::raii::Queue m_vkGraphicsQueue = nullptr;
    vk::raii::Queue m_vkPresentQueue = nullptr;
    vk::SurfaceFormatKHR m_vkSurfaceFormat = {};
    vk::Format m_vkSwapChainImageFormat = vk::Format::eUndefined;
    vk::Extent2D m_vkSwapExtent = {};
    vk::raii::SwapchainKHR m_vkSwapChain = nullptr;
    std::vector<vk::Image> m_vkSwapChainImages;
    std::vector<vk::raii::ImageView> m_vkSwapChainImageViews;
    vk::raii::CommandPool m_vkCommandPool = nullptr;
    vk::raii::CommandBuffer m_vkCommandBuffer = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_vkDebugMessenger = nullptr;

    void pickVkDevice();

    struct CheckDeviceResult
    {
        size_t graphicsQueueFamilyIndex = static_cast<size_t>(-1);
        size_t presentQueueFamilyIndex = static_cast<size_t>(-1);
        bool isSuitable = false;
        std::optional<vk::SurfaceFormatKHR> surfaceFormat = {};
    };

    [[nodiscard]]
    CheckDeviceResult checkVkDevice(const vk::PhysicalDevice &a_device, const std::vector<const char *> &a_requiredDeviceExtensions) const;

    void createVkSwapChain();

    void createVkImageViews();
};
