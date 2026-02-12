#pragma once
#include <vector>
#include <cstdint>

class BitReader
{
    const std::vector<uint8_t>& in;

    size_t index;   // current byte index in input
    uint8_t buffer;
    int bitsLeft;

public:
    explicit BitReader(const std::vector<uint8_t>& input, size_t startByte = 0);

    bool readBit(bool& bit);

    size_t bytesReadFromStart() const;

    bool atEnd() const;
};
