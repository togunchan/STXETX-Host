#include "../include/ReceiverThread.hpp"
#include "../include/RingBuffer.hpp"
#include "../include/Logger.hpp"

#include <unistd.h>
#include <iomanip>
#include <sstream>

void ReceiverThread::start(int serialFd, RingBuffer<1024> &ring)
{
    isRunning = true;
    readerThread = std::thread([this, serialFd, &ring]()
                               {
        while (isRunning)
        {
            Logger::debug("ReceiverThread::start() - Waiting for data...");
            uint8_t byte;
            ssize_t bytesRead = read(serialFd, &byte, 1);
            if (bytesRead > 0)
            {
                try
                {
                    Logger::debug("Received byte: " + std::to_string(bytesRead));
                    std::lock_guard<std::mutex> lock(mutex);
                    ring.push(byte);
                    
                    std::stringstream ss;
                    ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
                    Logger::info("The pushed byte is 0x" + ss.str());
                }
                catch (const std::exception &e)
                {
                    Logger::error(std::string("Ring buffer overflow") + e.what());
                    break;
                }
            }
            else if (bytesRead == 0)
            {
                Logger::debug("No data received");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            } 
            else if (bytesRead < 0)
            {
                Logger::error("Serial read error");
                break;
            }
        } });
}

void ReceiverThread::stop()
{
    Logger::info("Stopping receiver thread...");
    isRunning = false;
    if (readerThread.joinable())
    {
        readerThread.join(); // wait for the thread to finish
    }
}
