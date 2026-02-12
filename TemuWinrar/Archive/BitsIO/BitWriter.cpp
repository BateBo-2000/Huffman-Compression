#include "BitsWriter.h"

BitWriter::BitWriter(std::vector<uint8_t>& output)
    : out(output), buffer(0), bitsFilled(0)
{
}

void BitWriter::writeBit(bool bit)
{
    buffer = static_cast<uint8_t>((buffer << 1) | (bit ? 1 : 0));
    bitsFilled++;

    if (bitsFilled == 8)
    {
        out.push_back(buffer);
        buffer = 0;
        bitsFilled = 0;
    }
}

void BitWriter::writeBits(const std::vector<bool>& bits)
{
    for (size_t i = 0; i < bits.size(); i++)
        writeBit(bits[i]);
}

void BitWriter::flushZeroPad()
{
    if (bitsFilled == 0)
        return;

    buffer <<= (8 - bitsFilled);
    out.push_back(buffer);

    buffer = 0;
    bitsFilled = 0;
}

int BitWriter::pendingBits() const
{
    return bitsFilled;
}
