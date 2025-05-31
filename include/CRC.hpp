#pragma once

#include <vector>
#include <cstdint>

namespace SerialComm
{
    /**
     * @brief Compute a 16-bit CRC over the given data vector.
     *
     * @param data A vector of bytes to compute the CRC for.
     * @return The computed CRC-16 value.
     */
    uint16_t computeCRC16(const std::vector<uint8_t> &data);
}