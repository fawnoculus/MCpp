#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include "GLFW/glfw3.h"

#include "VulkanHandler.h"
#include "ResourceManager.h"

class Client final
{
public:
    static void addClientTask(const std::function<void(const Client &)> &a_task);

    Client();

    ~Client();

    [[nodiscard]]
    int run();

    void setFullscreen(bool a_fullscreen) const;

    void toggleFullscreen() const
    {
        setFullscreen(!m_fullscreen);
    };

    void onWindowClose(const GLFWwindow *a_glfwWindow);

    void onWindowResize(const GLFWwindow *a_glfwWindow, int a_width, int a_height);

private:
    std::shared_ptr<spdlog::logger> m_logger;
    VulkanHandler m_vulkanHandler = nullptr;
    ResourceManager m_resourceManager = nullptr;
    GraphicsPipeline m_testPipeline = nullptr;
    GLFWwindow *m_glfwWindow = nullptr;
    bool m_running = true;
    bool m_minimized = false;
    bool m_fullscreen = false;
};
