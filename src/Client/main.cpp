#define GLFW_INCLUDE_VULKAN

#include "Client.h"
#include "Logging.h"
#include "spdlog/spdlog.h"

int main()
{
    try
    {
        Client client;
        return client.run();
    } catch (std::exception &e)
    {
        spdlog::critical("An exception occurred while running Client: {}", e.what());
        spdlog::error("Terminating ...");
        spdlog::default_logger()->flush();
        return EXIT_FAILURE;
    }
}
