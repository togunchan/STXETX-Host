#include "FrameParser.hpp"
#include "FrameEncoder.hpp"
#include <iostream>

using namespace SerialComm;

int main()
{
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03};

    auto frame = encode(payload);

    FrameParser parser;
    std::optional<std::vector<uint8_t>> result;
    for (auto b : frame)
    {
        result = parser.push(b);
        if (result.has_value())
            break;
    }

    if (!result.has_value())
    {
        std::cerr << "ERROR: Parser did not return a payload!\n";
        return 1;
    }
    if (result.value() == payload)
    {
        std::cout << "SUCCESS: Parsed payload matches original\n";
        return 0;
    }
    else
    {
        std::cerr << "ERROR: Parsed payload differs:\n  got: ";
        for (auto b : result.value())
            std::cerr << int(b) << ' ';
        std::cerr << "\n";
        return 1;
    }
}