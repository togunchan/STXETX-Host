#pragma once

#include <string>

namespace SerialComm
{
    /**
     * @class Logger
     * @brief Simple wrapper around spdlog for file-based logging.
     */
    class Logger
    {
    public:
        /**
         * @brief Initialize the logger with a filename.
         * @param filename Log file path.
         */
        static void init(const std::string &filename);
        /**
         * @brief Log an info-level message.
         * @param msg The message to log.
         */
        static void info(const std::string &msg);
        /**
         * @brief Log a debug-level message.
         * @param msg The message to log.
         */
        static void debug(const std::string &msg);
        /**
         * @brief Log an error-level message.
         * @param msg The message to log.
         */
        static void error(const std::string &msg);
    };
} // namespace SerialComm
