// tests/test_FrameEncoder.cpp

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../include/FrameEncoder.hpp"
#include "../include/CRC.hpp"

///////////////////////////////////////////////////////////////////////////////
// TEST CASE 1: Empty payload should produce STX + escaped CRC(empty) + ETX
///////////////////////////////////////////////////////////////////////////////
static constexpr uint8_t STX = 0x02; // Start of Text (STX)
static constexpr uint8_t ETX = 0x03; // End of Text (ETX)
static constexpr uint8_t ESC = 0x1B; // Escape character (ESC)

TEST_CASE("FrameEncoder: encode empty payload", "[FrameEncoder]")
{
    // 1) Create an empty payload
    std::vector<uint8_t> emptyPayload;
    // 2) Call the encode function and retrieve the frame
    auto frame = SerialComm::encode(emptyPayload);

    // 3) Compute CRC for the empty payload
    uint16_t expectedCrc = SerialComm::computeCRC16(emptyPayload);
    uint8_t msb = static_cast<uint8_t>((expectedCrc >> 8) & 0xFF);
    uint8_t lsb = static_cast<uint8_t>(expectedCrc & 0xFF);

    // 4) Frame structure: [STX] [escaped MSB] [escaped LSB] [ETX]
    //    If MSB or LSB requires escaping (STX/ETX/ESC), the length increases by +1.
    //    Worst case scenario: both require escaping, total length = 1 + 2 + 2 + 1 = 6.
    //    Typically, CRC for empty data does not contain STX/ETX/ESC;
    //    in such cases, length = 1 + 1 + 1 + 1 = 4.
    REQUIRE(frame[0] == STX);

    size_t idx = 1;

    // 5) Check MSB: If escaping is needed -> ESC + (msb^0x20), otherwise directly msb
    if (msb == STX || msb == ETX || msb == ESC)
    {
        REQUIRE(frame[idx++] == ESC);
        REQUIRE(frame[idx++] == static_cast<uint8_t>(msb ^ 0x20));
    }
    else
    {
        REQUIRE(frame[idx++] == msb);
    }

    // 6) Check LSB: Same escaping rule applies
    if (lsb == STX || lsb == ETX || lsb == ESC)
    {
        REQUIRE(frame[idx++] == ESC);
        REQUIRE(frame[idx++] == static_cast<uint8_t>(lsb ^ 0x20));
    }
    else
    {
        REQUIRE(frame[idx++] == lsb);
    }

    // 7) Last byte must be ETX
    REQUIRE(frame[idx++] == ETX);

    // 8) idx should match the exact frame length
    REQUIRE(idx == frame.size());
}

///////////////////////////////////////////////////////////////////////////////
// TEST CASE 2: Simple payload without special bytes
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("FrameEncoder: encode simple payload", "[FrameEncoder]")
{
    // 1) Define a simple payload (no special bytes requiring escaping)
    std::vector<uint8_t> payload = {0x11, 0x22, 0x33};
    auto frame = SerialComm::encode(payload);

    // 2) Compute expected CRC value
    uint16_t expectedCrc = SerialComm::computeCRC16(payload);
    uint8_t msb = static_cast<uint8_t>((expectedCrc >> 8) & 0xFF);
    uint8_t lsb = static_cast<uint8_t>(expectedCrc & 0xFF);

    // 3) Expected frame length:
    //    [STX] + 3 payload bytes + (MSB) + (LSB) + [ETX] = 1 + 3 + 1 + 1 + 1 = 7
    size_t expectedSize = 1 + payload.size() + 2 + 1;
    REQUIRE(frame.size() == expectedSize);

    // 4) Verify frame contents sequentially
    size_t idx = 0;
    REQUIRE(frame[idx++] == STX);
    for (uint8_t b : payload)
    {
        REQUIRE(frame[idx++] == b);
    }
    REQUIRE(frame[idx++] == msb);
    REQUIRE(frame[idx++] == lsb);
    REQUIRE(frame[idx++] == ETX);

    // 5) idx should match the exact frame length
    REQUIRE(idx == frame.size());
}

///////////////////////////////////////////////////////////////////////////////
// TEST CASE 3: Payload containing special bytes (STX, ETX, ESC)
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("FrameEncoder: encode payload with special bytes", "[FrameEncoder]")
{
    // 1) Define a payload containing STX, ETX, and ESC values
    std::vector<uint8_t> payload = {STX, ETX, ESC, 0x55};

    auto frame = SerialComm::encode(payload);
    // 2) Compute expected CRC value
    uint16_t expectedCrc = SerialComm::computeCRC16(payload);
    uint8_t msb = static_cast<uint8_t>((expectedCrc >> 8) & 0xFF);
    uint8_t lsb = static_cast<uint8_t>(expectedCrc & 0xFF);

    // 3) First byte of the frame must be STX
    REQUIRE(frame[0] == STX);

    size_t idx = 1;
    // 4) Verify payload escaping rules manually:
    //    a) STX (within payload) => ESC, STX^0x20
    REQUIRE(frame[idx++] == ESC);
    REQUIRE(frame[idx++] == static_cast<uint8_t>(STX ^ 0x20));
    //    b) ETX (within payload) => ESC, ETX^0x20
    REQUIRE(frame[idx++] == ESC);
    REQUIRE(frame[idx++] == static_cast<uint8_t>(ETX ^ 0x20));
    //    c) ESC (within payload) => ESC, ESC^0x20
    REQUIRE(frame[idx++] == ESC);
    REQUIRE(frame[idx++] == static_cast<uint8_t>(ESC ^ 0x20));
    //    d) 0x55 (normal byte) => direct insertion
    REQUIRE(frame[idx++] == 0x55);

    // 5) Now check CRC MSB
    if (msb == STX || msb == ETX || msb == ESC)
    {
        REQUIRE(frame[idx++] == ESC);
        REQUIRE(frame[idx++] == static_cast<uint8_t>(msb ^ 0x20));
    }
    else
    {
        REQUIRE(frame[idx++] == msb);
    }

    // 6) Check CRC LSB
    if (lsb == STX || lsb == ETX || lsb == ESC)
    {
        REQUIRE(frame[idx++] == ESC);
        REQUIRE(frame[idx++] == static_cast<uint8_t>(lsb ^ 0x20));
    }
    else
    {
        REQUIRE(frame[idx++] == lsb);
    }

    // 7) Final byte must be ETX
    REQUIRE(frame[idx++] == ETX);

    // 8) idx should match the exact frame length
    REQUIRE(idx == frame.size());
}
