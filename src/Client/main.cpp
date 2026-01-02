#include "Client.h"
#include "spdlog/spdlog.h"

int main() {
    try {
        Client client;
        return client.run();
    } catch (std::exception &e) {
        spdlog::critical("An exception occurred while running Client: {}", e.what());
        return EXIT_FAILURE;
    }
}
