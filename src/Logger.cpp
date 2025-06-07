#include "../include/Logger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace SerialComm
{
    static std::shared_ptr<spdlog::logger> s_logger;

    void Logger::init(const std::string &filename)
    {
        s_logger = spdlog::basic_logger_mt("SerialCommLogger", filename);
        s_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        // Allow debug messages to be logged (default is info and above)
        s_logger->set_level(spdlog::level::debug);
        s_logger->flush_on(spdlog::level::info);
    }

    void Logger::info(const std::string &msg)
    {
        if (s_logger)
            s_logger->info(msg);
    }

    void Logger::debug(const std::string &msg)
    {
        if (s_logger)
            s_logger->debug(msg);
    }

    void Logger::error(const std::string &msg)
    {
        if (s_logger)
            s_logger->error(msg);
    }

}