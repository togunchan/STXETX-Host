#include "../include/Logger.hpp"
#include "../include/RingBuffer.hpp"
#include "../include/FrameParser.hpp"
#include "../include/FrameEncoder.hpp"
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>

using namespace SerialComm;

// Global serial port desciptor
static int serialFd = -1;

// Signal handler for SIGINT (Ctrl+C).
// The integer parameter represents the received signal, but it's unused in this function.
// It remains unnamed to avoid compiler warnings about usnused variables.
void cleanup(int)
{
    if (serialFd >= 0)
    {
        close(serialFd);
    }
    Logger::info("Serial port closed.");
    exit(0);
}

int main(int argc, char *argv[])
{
    Logger::init("serial_comm.log");
    Logger::info("Application started.");

    std::signal(SIGINT, cleanup);

    // open serial port
    const char *device = (argc > 1) ? argv[1] : "/dev/ttyUSB0";
    serialFd = open(device, O_RDWR | O_NOCTTY /*| O_NDELAY */);
    if (serialFd < 0)
    {
        Logger::error(std::string("Failed to open serial port: ") + device);
        return 1;
    }
    Logger::info(std::string("Opened serial port: ") + device);

    // tty stands for "teletype terminal"
    struct termios tty;
    // tcgetattr stand for "Terminal Control Get Attributes"
    if (tcgetattr(serialFd, &tty) < 0)
    {
        Logger::error("Failed to get terminal attributes.");
        return 1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits
    tty.c_cflag &= ~PARENB;                     // No parity
    tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                    // No hardware flow control
    tty.c_cflag |= (CLOCAL | CREAD);            // ignore modem controls, enable read
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // No software flow control
    tty.c_lflag = 0;                            // no echo, no signals
    tty.c_oflag = 0;                            // no remap, no delays
    tty.c_cc[VMIN] = 1;                         // read blocks until 1 byte arrives
    tty.c_cc[VTIME] = 1;                        // 0.1s read timeout

    if (tcsetattr(serialFd, TCSANOW, &tty) != 0)
    {
        Logger::error("tcsetattr failed");
        return 1;
    }
    Logger::info("Serial port configured: 115200 8N1");

    FrameParser parser;
    RingBuffer<1024> ring;

    // Read one byte at a time
    while (true)
    {
        uint8_t byte;
        ssize_t bytesRead = read(serialFd, &byte, 1);
        if (bytesRead < 0)
        {
            Logger::error("Serial read error");
            return 1;
        }
        if (bytesRead == 0)
        {
            continue;
        }

        try
        {
            ring.push(byte);
        }
        catch (const std::exception &e)
        {
            Logger::error(std::string("Ring buffer overflow") + e.what());
            return 1;
        }

        auto parsedData = parser.push(byte);

        if (parsedData.has_value())
        {
            const std::vector<uint8_t> &payload = parsedData.value();

            std::cout << "Received payload: ";
            for (uint8_t b : payload)
            {
                std::cout << " " << std::hex << static_cast<int>(b);
            }
            std::cout << std::dec << std::endl;
            Logger::info("Payload parsed successfully");

            auto ackFrame = encode({0x06});
            ssize_t bytesWritten = write(serialFd, ackFrame.data(), ackFrame.size());
            if (bytesWritten < 0)
            {
                Logger::error("Serial write error");
                return 1;
            }
            Logger::info("ACK frame sent");
        }
    }
    cleanup(0);
    return 0;
}