#include "../include/Logger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace SerialComm
{
    static std::shared_ptr<spdlog::logger> s_logger;

    void Logger::init(const std::string &filename)
    {
        // s_logger = spdlog::basic_logger_mt("SerialCommLogger", filename);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

        s_logger = std::make_shared<spdlog::logger>("SerialCommLogger", sinks.begin(), sinks.end());

        spdlog::register_logger(s_logger);
        spdlog::set_default_logger(s_logger);

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