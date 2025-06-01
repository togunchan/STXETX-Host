#pragma once

#include <vector>
#include <cstdint>
#include <optional>

namespace SerialComm
{
    /**
     * @enum ParserState
     * @brief The three possible states of the parser: waiting for STX, inside a frame, and escape processing.
     */
    enum class ParserState
    {
        WAIT_STX,
        IN_FRAME,
        IN_ESCAPE
    };

    class FrameParser
    {
    public:
        FrameParser();
        ~FrameParser() = default;
        /**
         * @brief Passes an incoming byte to the parser.
         * @param byte The received byte.
         * @return Returns the payload if a valid frame is completed; otherwise, std::nullopt.
         */
        std::optional<std::vector<uint8_t>> push(uint8_t byte);

        /**
         * @brief Resets the parser to the initial state (WAIT_STX) and clears the buffer.
         */
        void reset();

    private:
        ParserState state_;
        std::vector<uint8_t> buffer_;
        static constexpr uint8_t STX = 0x02;
        static constexpr uint8_t ETX = 0x03;
        static constexpr uint8_t ESC = 0x1B;

        /**
         * @brief Retrieves the last two bytes from the buffer as the CRC and compares them with the computed CRC.
         * @return Returns true if the CRC is valid, otherwise false.
         */
        bool validateCRC();
    };
} // namespace SerialComm