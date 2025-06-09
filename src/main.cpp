#include "../include/Logger.hpp"
#include "../include/RingBuffer.hpp"
#include "../include/FrameParser.hpp"
#include "../include/FrameEncoder.hpp"
#include "../include/ReceiverThread.hpp"
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <string>
#include <iomanip>

using namespace SerialComm;

// Global serial port desciptor
static int serialFd = -1;

ReceiverThread *gReceiverPtr = nullptr;
volatile std::sig_atomic_t running = 1;

// Signal handler for SIGINT (Ctrl+C).
// The integer parameter represents the received signal, but it's unused in this function.
// It remains unnamed to avoid compiler warnings about usnused variables.
void cleanup(int)
{
    running = 0;
}

int main(int argc, char *argv[])
{
    Logger::init("serial_comm.log");
    Logger::info("Application started.");

    // open serial port
    const char *device = (argc > 1) ? argv[1] : "/dev/ttyUSB0";
    serialFd = open(device, O_RDWR | O_NOCTTY /*| O_NDELAY */);
    tcflush(serialFd, TCIFLUSH); // Flush input buffer
    Logger::info("Serial input buffer flushed.");
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
    tcflush(serialFd, TCIFLUSH); // Flush input buffer
    Logger::info("Serial input buffer flushed.");
    Logger::info("Serial port configured: 115200 8N1");

    FrameParser parser;
    RingBuffer<1024> ring;
    ReceiverThread receiver;
    gReceiverPtr = &receiver;

    std::signal(SIGINT, cleanup);

    receiver.start(serialFd, ring);

    while (running)
    {
        uint8_t byte;
        try
        {
            byte = ring.pop();
            std::stringstream ss;
            ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            Logger::info("The popped byte is 0x" + ss.str());
        }
        catch (const std::exception &e)
        {
            Logger::debug("Ring buffer empty");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        std::stringstream ssHex;
        ssHex << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        Logger::debug("Feeding parser byte 0x" + ssHex.str() + " (" + std::to_string(static_cast<int>(byte)) + ")");
        auto parsedData = parser.push(byte);

        Logger::debug("byte parsed into parsedData");
        Logger::debug("parsedData has value" + std::to_string(parsedData.has_value()));
        if (parsedData.has_value())
        {
            const std::vector<uint8_t> &payload = parsedData.value();

            for (uint8_t b : payload)
            {
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(b);
                Logger::debug("The read payload byte is 0x" + ss.str());
            }
            Logger::info("Payload parsed successfully");

            auto ackFrame = encode({0x06});
            ssize_t bytesWritten = write(serialFd, ackFrame.data(), ackFrame.size());
            if (bytesWritten < 0)
            {
                Logger::error("Serial write error");
                return 1;
            }
            else if (bytesWritten == ackFrame.size())
            {
                std::ostringstream oss;
                oss << std::hex << std::uppercase;
                for (auto byte : ackFrame)
                    oss << " 0x" << std::setw(2) << std::setfill('0') << int(byte);
                Logger::info("Sent ACK bytes:" + oss.str());
            }
            else
            {
                Logger::error("Partial ACK write: " + std::to_string(bytesWritten));
            }
            Logger::info("ACK frame sent");
        }
    }
    receiver.stop();

    if (serialFd >= 0)
    {
        close(serialFd);
        Logger::info("Serial port closed");
    }

    Logger::info("Application stopped.");
    return 0;
}