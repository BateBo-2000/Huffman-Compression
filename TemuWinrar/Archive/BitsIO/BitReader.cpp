#include "BitReader.h"

BitReader::BitReader(const std::vector<uint8_t>& input, size_t startByte)
    : in(input), index(startByte), buffer(0), bitsLeft(0)
{
}

bool BitReader::readBit(bool& bit)
{
    if (bitsLeft == 0)
    {
        if (index >= in.size())
            return false;

        buffer = in[index++];
        bitsLeft = 8;
    }

    bit = (buffer & 0x80) != 0;
    buffer <<= 1;
    --bitsLeft;

    return true;
}

size_t BitReader::bytesReadFromStart() const
{
    return index;
}

bool BitReader::atEnd() const
{
    return (index >= in.size()) && (bitsLeft == 0);
}
