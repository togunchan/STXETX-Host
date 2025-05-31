#include <catch2/catch_all.hpp>
#include "../src/CRC.cpp"

TEST_CASE("Empty vector yields 0xFFFF (initial CRC)", "[CRC]")
{
    std::vector<uint8_t> empty_data;
    uint16_t crc = SerialComm::computeCRC16(empty_data);
    REQUIRE(crc == 0xFFFF);
}

TEST_CASE("Known data '123456789' yields CRC-0x29B1", "[CRC]")
{
    std::vector<uint8_t> data = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint16_t crc = SerialComm::computeCRC16(data);
    REQUIRE(crc == 0x29B1);
}
