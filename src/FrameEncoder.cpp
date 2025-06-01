#include "FrameEncoder.hpp"
#include "CRC.hpp" // To use CRC::compute

namespace SerialComm
{
    // Fixed byte values according to the protocol
    static constexpr uint8_t STX = 0x02; // Start of Text (STX)
    static constexpr uint8_t ETX = 0x03; // End of Text (ETX)
    static constexpr uint8_t ESC = 0x1B; // Escape character (ESC)

    // `constexpr`: Ensures the variable is evaluated at compile-time.
    // `static`: Limits the variable's visibility to this translation unit.
    // Using `static constexpr` prevents ODR (One Definition Rule) violations
    // when the header file is included in multiple translation units, ensuring
    // each translation unit has its own copy of the variable.

    // If a byte equals STX, ETX, or ESC, escape it; otherwise just push it.
    static void appendEscaped(std::vector<uint8_t> &out, uint8_t byte)
    {
        if (byte == STX || byte == ETX || byte == ESC)
        {
            out.push_back(ESC);
            out.push_back(byte ^ 0x20);
        }
        else
        {
            out.push_back(byte);
        }
    }

    std::vector<uint8_t> encode(const std::vector<uint8_t> &payload)
    {
        std::vector<uint8_t> frame;
        // Preallocate memory for the frame to prevent multiple reallocations.
        // Calculation:
        // - 1 byte for STX (Start of Text)
        // - Maximum of `payload.size() * 2` bytes (each byte might be escaped)
        // - 2 bytes for CRC (MSB & LSB), each potentially escaped (2 * 2)
        // - 1 byte for ETX (End of Text)
        frame.reserve(1 + payload.size() * 2 + 2 * 2 + 1);

        frame.push_back(STX);
        for (uint8_t byte : payload)
        {
            appendEscaped(frame, byte);
        }

        uint16_t crc = SerialComm::computeCRC16(payload);

        // `crc >> 8` shifts the value right by 8 bits, bringing the upper 8 bits forward.
        // `& 0xFF` ensures that only the lower 8 bits of the shifted value remain, eliminating extra bits.
        // This extracts the MSB of the CRC
        uint8_t msb = static_cast<uint8_t>((crc >> 8) & 0xFF);
        // `crc & 0xFF` directly masks the lower 8 bits of the 16-bit value.
        // This extracts the LSB of the CRC.
        uint8_t lsb = static_cast<uint8_t>(crc & 0xFF);
        appendEscaped(frame, msb);
        appendEscaped(frame, lsb);

        frame.push_back(ETX);
        return frame;
    }
} // namespace SerialComm
