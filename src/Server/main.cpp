#include "DedicatedServer.h"
#include "spdlog/spdlog.h"

int main() {
    try {
        DedicatedServer server;
        return server.run();
    } catch (std::exception &e) {
        spdlog::critical("An exception occurred while running Client: {}", e.what());
        return EXIT_FAILURE;
    }
}
