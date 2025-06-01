#pragma once

#include <vector>
#include <cstdint>

namespace SerialComm
{
    /**
     * @brief Encode raw payload data into a framed message using STX/ETX/ESC and CRC-16.
     *
     * @param payload The raw data to be framed.
     * @return A vector of bytes representing the complete frame:
     *         STX + escaped payload + CRC(MSB+LSB, escaped if needed) + ETX.
     */
    std::vector<uint8_t> encode(const std::vector<uint8_t> &payload);
} // namespace SerialComm
