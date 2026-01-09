#include <cmath>
#include <stdfloat>

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

#include "Logging.h"
#include "Client.h"

using std::string, std::vector, std::optional, std::shared_ptr, std::float64_t;

constexpr int windowWidth = 854;
constexpr int windowHeight = 480;
shared_ptr<spdlog::logger> g_glfwLogger = nullptr;
Client *g_client = nullptr;
std::mutex g_clientTasksMutex;
vector<std::function<void(const Client&)>> g_clientTasks;

void glfwError(const int a_errorCode, const char *a_description)
{
    if (g_glfwLogger == nullptr) return;
    g_glfwLogger->error("{}: {}", a_errorCode, a_description);
}

void glfwWindowClose(GLFWwindow *a_glfwWindow)
{
    if (g_client != nullptr)
    {
        g_client->onWindowClose(a_glfwWindow);
    }
}

void glfwWindowResize(GLFWwindow *a_glfwWindow, const int a_width, const int a_height)
{
    if (g_client != nullptr)
    {
        g_client->onWindowResize(a_glfwWindow, a_width, a_height);
    }
}

void Client::addClientTask(const std::function<void(const Client &)> &a_task)
{
    std::lock_guard guard(g_clientTasksMutex);
    g_clientTasks.push_back(a_task);
}

Client::Client()
{
    g_client = this;

    Logging::setupLogging();

    m_logger = Logging::getLogger("Client");
    m_logger->info("Initializing Client ...");

    g_glfwLogger = Logging::getLogger("GLFW");

    m_logger->debug("Initializing GLFW");
    if (glfwInit() != GLFW_TRUE)
    {
        m_logger->error("Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(glfwError);

    if (glfwVulkanSupported() != GLFW_TRUE)
    {
        m_logger->error("Current GLFW Doesn't support Vulkan");
        throw std::runtime_error("Current GLFW Doesn't support Vulkan");
    }

    m_resourceManager = ResourceManager(m_logger, std::filesystem::current_path().append("resources"));

    m_vulkanHandler = VulkanHandler(m_logger);
    m_vulkanHandler.initialize();
    m_vulkanHandler.setResourceManager(&m_resourceManager);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_logger->debug("Creating Window");
    m_glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "MCpp", nullptr, nullptr);
    if (!m_glfwWindow)
    {
        m_logger->error("Failed to create Window");
        throw std::runtime_error("Failed to create Window");
    }
    glfwSetWindowSizeLimits(m_glfwWindow, 1, 1, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowCloseCallback(m_glfwWindow, glfwWindowClose);
    glfwSetWindowSizeCallback(m_glfwWindow, glfwWindowResize);
    m_vulkanHandler.setWindow(m_glfwWindow);
    glfwShowWindow(m_glfwWindow);

    m_logger->info("Finished Client initialisation");
}

Client::~Client()
{
    m_logger->info("Stoping Client ...");

    m_logger->debug("Destroying Window");
    glfwDestroyWindow(m_glfwWindow);

    m_logger->debug("Terminating GLFW");
    glfwTerminate();

    m_logger->flush();
}

int Client::run() const
{
    m_logger->info("Running Client ...");

    while (m_running)
    {
        if (m_minimized)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            glfwPollEvents();
            continue;
        }

        std::chrono::time_point<std::chrono::system_clock> frameStart = std::chrono::high_resolution_clock::now();
        glfwPollEvents();

        {
            std::lock_guard guard(g_clientTasksMutex);
            for (std::function clientTask : g_clientTasks)
            {
                clientTask(*this);
            }
            g_clientTasks.clear();
        }

        // TODO: do rendering here

        std::chrono::time_point<std::chrono::system_clock> frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float64_t> elapsed_seconds = frameEnd - frameStart;
        const float64_t fps = 1.f / elapsed_seconds.count();
        glfwSetWindowTitle(m_glfwWindow, std::format("MCpp @{}FPS", round(fps)).c_str());
    }

    return EXIT_SUCCESS;
}

void Client::setFullscreen(const bool a_fullscreen) const
{
    if (a_fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else
    {
        glfwSetWindowMonitor(m_glfwWindow, nullptr, 0, 0, windowWidth, windowHeight, GLFW_DONT_CARE);
    }
}

void Client::onWindowClose(const GLFWwindow *a_glfwWindow)
{
    if (a_glfwWindow != m_glfwWindow)
        return;

    glfwPostEmptyEvent();
    m_running = false;
}

void Client::onWindowResize(const GLFWwindow *a_glfwWindow, const int a_width, const int a_height)
{
    if (a_glfwWindow != m_glfwWindow)
        return;

    glfwPostEmptyEvent();
    if (a_width == 0 || a_height == 0)
    {
        m_minimized = true;
        return;
    }
    m_minimized = false;

    m_vulkanHandler.onWindowResize(a_glfwWindow, a_width, a_height);
}
