#pragma once
#include "../include/RingBuffer.hpp"

#include <thread>
#include <mutex>

using namespace SerialComm;

class ReceiverThread
{
public:
    void start(int serialFd, RingBuffer<1024> &ring);
    void stop();

private:
    std::atomic<bool> isRunning;
    std::thread readerThread;
    std::mutex mutex;
};