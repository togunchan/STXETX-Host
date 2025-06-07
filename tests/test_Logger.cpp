#include "../include/Logger.hpp"
#include <fstream>
#include <cassert>

int main()
{
    const std::string logFile = "test.log";
    SerialComm::Logger::init(logFile);

    SerialComm::Logger::info("This is an info message");
    SerialComm::Logger::debug("This is a debug message");
    SerialComm::Logger::error("This is an error message");

    // Verifiy log file was created and contains at least one line
    std::ifstream file(logFile);
    assert(file.is_open()); // If the file could not be opened, abort the program
    std::string line;
    bool hasLine = false;
    while (std::getline(file, line))
    {
        hasLine = true;
        break;
    }
    assert(hasLine); // Ensures the log file contains at least one message

    return 0;
}