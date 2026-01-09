#pragma once

#include <memory>

#include "spdlog/spdlog.h"

class DedicatedServer final
{
public:
    DedicatedServer();

    ~DedicatedServer();

    [[nodiscard]]
    int run() const;

private:
    std::shared_ptr<spdlog::logger> m_logger;
};
