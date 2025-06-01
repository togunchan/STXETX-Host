#include "FrameParser.hpp"
#include "CRC.hpp"

namespace SerialComm
{
    FrameParser::FrameParser() : state_(ParserState::WAIT_STX)
    {
        buffer_.reserve(512);
    }

    void FrameParser::reset()
    {
        buffer_.clear();
        state_ = ParserState::WAIT_STX;
    }

    std::optional<std::vector<uint8_t>> FrameParser::push(uint8_t byte)
    {
        switch (state_)
        {
        case ParserState::WAIT_STX:
            if (byte == STX)
            {
                state_ = ParserState::IN_FRAME;
                buffer_.clear();
            }
            return std::nullopt;
        case ParserState::IN_FRAME:
            if (byte == ESC)
            {
                state_ = ParserState::IN_ESCAPE;
            }
            else if (byte == ETX)
            {
                if (validateCRC())
                {
                    // extracting payload without CRC
                    size_t payloadSize = buffer_.size() - 2;
                    std::vector<uint8_t> payload(buffer_.begin(), buffer_.begin() + payloadSize);
                    reset();
                    return payload;
                }
                else
                {
                    reset();
                    return std::nullopt;
                }
            }
            else
            {
                buffer_.push_back(byte);
            }
            return std::nullopt;
        case ParserState::IN_ESCAPE:
            buffer_.push_back(byte ^ 0x20);
            //"Obtain the original value of the received byte by applying XOR with 0x20."
            state_ = ParserState::IN_FRAME;
            // We haven't reached the end of the frame (ETX) yet, so no valid payload is available.
            // Returning std::nullopt signals that parsing is ongoing, and more bytes are needed.
            return std::nullopt;
        }
        return std::nullopt;
    }

    bool FrameParser::validateCRC()
    {
        if (buffer_.size() < 2)
        {
            return false;
        }

        // get last two bytes as CRC
        size_t n = buffer_.size();
        uint8_t msb = buffer_[n - 2];
        uint8_t lsb = buffer_[n - 1];
        uint16_t receivedCRC = static_cast<uint16_t>(msb) << 8 | static_cast<uint16_t>(lsb);

        // get payload without last two bytes
        std::vector<uint8_t> payload(buffer_.begin(), buffer_.end() - 2);
        uint16_t computedCRC = SerialComm::computeCRC16(payload);

        return receivedCRC == computedCRC;
    }

} // namespace SerialComm