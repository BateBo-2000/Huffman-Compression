#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

#include "Archive/Compression/HuffmanCompressor.h"

static std::vector<uint8_t> buildBigInput()
{
    std::vector<uint8_t> data;

    std::string base =
        "Huffman compression works best when symbol frequencies are uneven. "
        "This is a long repeated paragraph to simulate real text data. "
        "We want letters like e, t, a, o, i, n to appear often, "
        "while rare symbols appear less often.\n";

    for (int i = 0; i < 800; ++i)
        data.insert(data.end(), base.begin(), base.end());


    for (int c = 0; c < 256; ++c)
    {
        int repeat = (c % 17) + 1;
        for (int r = 0; r < repeat; ++r)
            data.push_back((uint8_t)c);
    }


    for (int i = 0; i < 5000; ++i) data.push_back('e');
    for (int i = 0; i < 3000; ++i) data.push_back('t');
    for (int i = 0; i < 2000; ++i) data.push_back(' ');
    for (int i = 0; i < 1200; ++i) data.push_back('\n');

    return data;
}

int main()
{
    HuffmanCompressor hc;

    std::vector<uint8_t> input = buildBigInput();

    std::cout << "Input size = " << input.size() << " bytes\n\n";

    hc.debugPrintDictionary(input);

    return 0;
}