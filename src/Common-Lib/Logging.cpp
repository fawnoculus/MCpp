#include "spdlog/spdlog.h"
#include "spdlog/async.h"

#include "Logging.h"

namespace Logging {
    std::shared_ptr<spdlog::logger> makeLoggerOrThrow(
        const std::string &name,
        const std::shared_ptr<spdlog::details::thread_pool> &threadPool,
        const spdlog::async_overflow_policy overflowPolicy
    ) {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.reserve(4);
        sinks.push_back(stdoutSink);
        sinks.push_back(debugLogSink);
        sinks.push_back(latestLogSink);
        sinks.push_back(dateLogSink);

        const auto logger = std::make_shared<spdlog::async_logger>(
            name, sinks.begin(), sinks.end(),
            threadPool, overflowPolicy
        );
        logger->set_pattern("[%T] %^[thread %t/%l]%$ (%n) %v");
        logger->set_level(spdlog::level::debug);

        spdlog::register_logger(logger);
        return logger;
    }

    std::shared_ptr<spdlog::logger> getLogger(const std::string &name) {
        auto logger = spdlog::get(name);
        if (!logger) {
            logger = makeLoggerOrThrow(name, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }

        return logger;
    }


    void setupLogging() {
        if (isInitialized) return;
        isInitialized = true;

        spdlog::init_thread_pool(65536, 1);

        stdoutSink->set_level(spdlog::level::debug);
        debugLogSink->set_level(spdlog::level::debug);
        latestLogSink->set_level(spdlog::level::info);
        dateLogSink->set_level(spdlog::level::info);

        const auto logger = getLogger("Global");
        spdlog::set_default_logger(logger);

        logger->debug("Logger initialized");
    }
}
