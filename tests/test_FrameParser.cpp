// tests/test_FrameParser.cpp

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "FrameEncoder.hpp"
#include "FrameParser.hpp"

using namespace SerialComm;

///////////////////////////////////////////////////////////////////////////////
// TEST CASE: FrameEncoder and FrameParser should work together
///////////////////////////////////////////////////////////////////////////////
static constexpr uint8_t STX = 0x02;
static constexpr uint8_t ETX = 0x03;
static constexpr uint8_t ESC = 0x1B;

TEST_CASE("FrameParser: parse frame produced by FrameEncoder", "[FrameParser]")
{
    // 1) Example payload: includes both normal bytes and escape-required bytes
    std::vector<uint8_t> originalPayload = {0x11, STX, 0x22, ESC, 0x33, ETX, 0x44};

    // 2) Encode this payload into a frame using FrameEncoder
    auto encodedFrame = encode(originalPayload);

    // 3) Create a FrameParser instance
    FrameParser parser;

    // 4) Push each byte from the encoded frame into the parser and check the result
    std::optional<std::vector<uint8_t>> maybePayload;
    for (uint8_t b : encodedFrame)
    {
        maybePayload = parser.push(b);
        // If a payload is obtained (std::optional contains a value), break the loop
        if (maybePayload.has_value())
        {
            break;
        }
    }

    // 5) maybePayload must contain a value (parsing should be complete)
    REQUIRE(maybePayload.has_value());

    // 6) The extracted payload should be identical to the originalPayload
    const auto &parsedPayload = maybePayload.value();
    REQUIRE(parsedPayload.size() == originalPayload.size());
    for (size_t i = 0; i < originalPayload.size(); ++i)
    {
        REQUIRE(parsedPayload[i] == originalPayload[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////
// TEST CASE: Parser should reset and not return payload if CRC is invalid
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("FrameParser: CRC mismatch should not return payload", "[FrameParser]")
{
    // 1) Simple payload
    std::vector<uint8_t> payload = {0x55, 0x66, 0x77};

    // 2) Generate an encoded frame
    auto frame = encode(payload);

    // 3) Intentionally corrupt the CRC bytes in the frame
    //    (e.g., invert the last byte)
    if (frame.size() >= 3)
    {
        frame[frame.size() - 2] ^= 0xFF; // Corrupt MSB
    }

    // 4) Create a FrameParser instance
    FrameParser parser;

    // 5) Push the corrupted frame byte-by-byte
    std::optional<std::vector<uint8_t>> maybePayload;
    for (uint8_t b : frame)
    {
        maybePayload = parser.push(b);
        // In case of an error, payload should never be returned
        REQUIRE(!maybePayload.has_value());
    }

    // 6) At the end of the loop, the parser should not return any payload
    REQUIRE(!maybePayload.has_value());
}

///////////////////////////////////////////////////////////////////////////////
// TEST CASE: Escape sequences should be processed correctly
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("FrameParser: payload with escaped bytes", "[FrameParser]")
{
    // 1) Payload contains STX, ETX, and ESC characters
    std::vector<uint8_t> payload = {STX, ETX, ESC, 0x99};

    // 2) Encode this payload
    auto frame = encode(payload);

    // 3) Create a parser and feed bytes one by one
    FrameParser parser;
    std::optional<std::vector<uint8_t>> maybePayload;
    for (uint8_t b : frame)
    {
        maybePayload = parser.push(b);
        if (maybePayload.has_value())
        {
            break;
        }
    }

    // 4) At the end, the payload should be returned
    REQUIRE(maybePayload.has_value());

    // 5) The payload extracted from the parser should match the original payload exactly
    const auto &parsed = maybePayload.value();
    REQUIRE(parsed.size() == payload.size());
    for (size_t i = 0; i < payload.size(); ++i)
    {
        REQUIRE(parsed[i] == payload[i]);
    }
}
