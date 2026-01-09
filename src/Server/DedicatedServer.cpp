#include "spdlog/spdlog.h"
#include "../Common-Lib/Logging.h"
#include "DedicatedServer.h"

DedicatedServer::DedicatedServer()
{
    Logging::setupLogging();
    m_logger = Logging::getLogger("Server");

    m_logger->info("Starting Server ...");
};

DedicatedServer::~DedicatedServer()
{
    m_logger->info("Stopping Server ...");

    m_logger->flush();
};

int DedicatedServer::run() const
{
    m_logger->info("Running Server ...");
    m_logger->warn("I decided, i didn't feel like doing anything, fuck u");

    return EXIT_SUCCESS;
}
