#include "CRC.hpp"

namespace SerialComm
{
    /**
     * @brief Calculate bitwise CRC-16-CCITT (poly = 0x1021) over data.
     *
     * @param data Input byte vector.
     * @return 16-bit CRC value.
     */
    uint16_t computeCRC16(const std::vector<uint8_t> &data)
    {
        // Initialize CRC to 0xFFFF (common initial value for CRC-CCITT)
        uint16_t crc = 0xFFFF;
        const uint16_t polynomial = 0x1021;

        // Loop through each byte
        for (uint8_t byte : data)
        {
            crc ^= static_cast<uint16_t>(byte) << 8; // Bring in the byte

            // Process each bit
            for (int bit = 0; bit < 8; ++bit)
            {
                if (crc & 0x8000)
                {
                    crc = static_cast<uint16_t>((crc << 1) ^ polynomial);
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }
} // namespace SerialComm