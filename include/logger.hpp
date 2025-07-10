#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <memory>

// Define aliases for ease
#define trace(...) SPDLOG_LOGGER_TRACE(Logger::get(), __VA_ARGS__)
#define debug(...) SPDLOG_LOGGER_DEBUG(Logger::get(), __VA_ARGS__)
#define info(...)  SPDLOG_LOGGER_INFO(Logger::get(), __VA_ARGS__)
#define warn(...)  SPDLOG_LOGGER_WARN(Logger::get(), __VA_ARGS__)
#define err(...)   SPDLOG_LOGGER_ERROR(Logger::get(), __VA_ARGS__)
#define crit(...)  SPDLOG_LOGGER_CRITICAL(Logger::get(), __VA_ARGS__)

class Logger {
public:
    static std::shared_ptr<spdlog::logger>& get() {
        static std::shared_ptr<spdlog::logger> logger = create();
        return logger;
    }

private:
    static std::shared_ptr<spdlog::logger> create() {
        // Use thread pool with 8k queue and 1 worker thread
        spdlog::init_thread_pool(8192, 1);

        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sink->set_pattern("[%T.%e] [%^%l%$] %v");

        auto logger = std::make_shared<spdlog::async_logger>(
            "syncwave",
            sink,
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        logger->set_level(spdlog::level::trace); // Or info/debug etc.
        spdlog::register_logger(logger);
        return logger;
    }
};
