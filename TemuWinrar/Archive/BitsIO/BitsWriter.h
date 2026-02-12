#pragma once
#include <vector>
#include <cstdint>

class BitWriter
{
    std::vector<uint8_t>& out;

    uint8_t buffer;     // 1 byte
    int bitsFilled;     // 0-7

public:
    explicit BitWriter(std::vector<uint8_t>& output);


    void writeBit(bool bit);


    void writeBits(const std::vector<bool>& bits);

    // padding
    void flushZeroPad();

    // unflushed bits (buffer)
    int pendingBits() const;
};