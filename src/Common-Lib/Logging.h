#pragma once

#include <filesystem>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"

namespace Logging {
    // Setup logging sinks (aka: log files) & the logger pattern
    void setupLogging();

    // Get a logger by name or create it if it doesn't already exist
    [[nodiscard]]
    std::shared_ptr<spdlog::logger> getLogger(const std::string &name);

    // Create a logger & register it, will throw an exception if a logger with that name already exist
    [[nodiscard]]
    std::shared_ptr<spdlog::logger> makeLoggerOrThrow(
        const std::string &name,
        const std::shared_ptr<spdlog::details::thread_pool> &threadPool,
        spdlog::async_overflow_policy overflowPolicy
    );

    // Create a logger & register it, will throw an exception if a logger with that name already exist
    [[nodiscard]]
    inline std::shared_ptr<spdlog::logger> makeLoggerOrThrow(const std::string &name) {
        return makeLoggerOrThrow(name, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    }

    // Create a logger & register it, will throw an exception if a logger with that name already exist
    [[nodiscard]]
    inline std::shared_ptr<spdlog::logger> makeLoggerOrThrow(
        const std::string &name,
        const std::shared_ptr<spdlog::details::thread_pool> &threadPool
    ) {
        return makeLoggerOrThrow(name, threadPool, spdlog::async_overflow_policy::block);
    }

    // Create a logger & register it, will throw an exception if a logger with that name already exist
    [[nodiscard]]
    inline std::shared_ptr<spdlog::logger> makeLoggerOrThrow(
        const std::string &name,
        const spdlog::async_overflow_policy overflowPolicy
    ) {
        return makeLoggerOrThrow(name, spdlog::thread_pool(), overflowPolicy);
    }

    [[nodiscard]]
    constexpr std::shared_ptr<spdlog::sinks::basic_file_sink_mt> makeDateSink() {
        const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const std::tm *localtimeNow = std::localtime(&now);

        char buffer[256];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtimeNow);
        std::string date = buffer;

        int32_t extraDigit = 1;
        while (std::filesystem::exists(std::format("logs/{}-{}.log", date, extraDigit))) {
            extraDigit++;
        }

        return std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            std::format("logs/{}-{}.log", date, extraDigit),
            true
        );
    }

    inline const auto latestLogSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/latest.log", true);
    inline const auto debugLogSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/debug.log", true);
    inline const auto dateLogSink = makeDateSink();
    inline const auto stdoutSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    inline bool isInitialized = false;
}
