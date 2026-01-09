#include "DedicatedServer.h"
#include "spdlog/spdlog.h"

int main()
{
    try
    {
        DedicatedServer server;
        return server.run();
    } catch (std::exception &e)
    {
        spdlog::critical("An exception occurred while running Server: {}", e.what());
        spdlog::error("Terminating ...");
        spdlog::default_logger()->flush();
        return EXIT_FAILURE;
    }
}
