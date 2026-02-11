#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Archive/Compression/HuffmanCompressor.h"

// ---------- helpers ----------
static std::string captureStdout(void (*fn)(HuffmanCompressor&, const std::vector<uint8_t>&),
    HuffmanCompressor& hc,
    const std::vector<uint8_t>& input)
{
    std::ostringstream oss;
    auto* old = std::cout.rdbuf(oss.rdbuf());
    fn(hc, input);
    std::cout.rdbuf(old);
    return oss.str();
}

struct DictRow {
    uint64_t freq = 0;
    std::string code;
    std::string ch; // printed token, "." or "A" etc.
};

static std::unordered_map<int, DictRow> parsePrintedDictionary(const std::string& out)
{
    std::unordered_map<int, DictRow> rows;

    std::istringstream iss(out);
    std::string line;

    while (std::getline(iss, line)) {
        // Data lines start with a number (byte value). Header lines don't.
        // We'll try to parse: <byte> <charToken> <freq> <codeBits>
        std::istringstream ls(line);
        int byte = -1;
        std::string chTok;
        uint64_t freq = 0;
        std::string code;

        if (!(ls >> byte >> chTok >> freq)) continue; // not a data line

        // code may be empty in broken output; read if present
        if (!(ls >> code)) code = "";

        DictRow r;
        r.freq = freq;
        r.code = code;
        r.ch = chTok;

        rows[byte] = r;
    }

    return rows;
}

static uint64_t parseOriginalSize(const std::string& out)
{
    // looks like: "Original size: <number>"
    std::istringstream iss(out);
    std::string line;
    while (std::getline(iss, line)) {
        const std::string key = "Original size:";
        auto pos = line.find(key);
        if (pos != std::string::npos) {
            std::istringstream ls(line.substr(pos + key.size()));
            uint64_t sz = 0;
            ls >> sz;
            return sz;
        }
    }
    // if missing, force fail in tests
    return UINT64_MAX;
}

static bool isPrefix(const std::string& a, const std::string& b)
{
    if (a.size() > b.size()) return false;
    return std::equal(a.begin(), a.end(), b.begin());
}

// wrapper to pass into captureStdout
static void callDebug(HuffmanCompressor& hc, const std::vector<uint8_t>& input)
{
    hc.debugPrintDictionary(input);
}


// ---------- tests ----------
static void test_empty_input()
{
    HuffmanCompressor hc;
    std::vector<uint8_t> input;

    std::string out = captureStdout(&callDebug, hc, input);

    // basic structure
    assert(out.find("===== HUFFMAN DICTIONARY =====") != std::string::npos);
    assert(out.find("Original size: 0") != std::string::npos);

    auto rows = parsePrintedDictionary(out);
    assert(rows.empty()); // no symbols printed
}

static void test_single_symbol_code_is_0()
{
    HuffmanCompressor hc;
    std::vector<uint8_t> input(7, (uint8_t)'A'); // "AAAAAAA"

    std::string out = captureStdout(&callDebug, hc, input);

    assert(parseOriginalSize(out) == input.size());

    auto rows = parsePrintedDictionary(out);
    assert(rows.size() == 1);
    assert(rows.count((int)'A') == 1);

    const auto& r = rows[(int)'A'];
    assert(r.freq == 7);

    // Your buildCodes() forces single-symbol code to be "0"
    assert(r.code == "0");
}

static void test_two_symbols_have_freqs_and_prefix_free_codes()
{
    HuffmanCompressor hc;
    std::vector<uint8_t> input;
    input.push_back((uint8_t)'A');
    input.push_back((uint8_t)'B');

    std::string out = captureStdout(&callDebug, hc, input);

    assert(parseOriginalSize(out) == input.size());

    auto rows = parsePrintedDictionary(out);
    assert(rows.size() == 2);
    assert(rows.count((int)'A') == 1);
    assert(rows.count((int)'B') == 1);

    assert(rows[(int)'A'].freq == 1);
    assert(rows[(int)'B'].freq == 1);

    const std::string& cA = rows[(int)'A'].code;
    const std::string& cB = rows[(int)'B'].code;

    // Usually "0" and "1" (order can vary depending on heap), but at least:
    assert(!cA.empty() && !cB.empty());
    assert(cA != cB);

    // Prefix-free check
    assert(!isPrefix(cA, cB));
    assert(!isPrefix(cB, cA));

    // With only 2 symbols, both codes should be length 1 in classic Huffman
    assert(cA.size() == 1);
    assert(cB.size() == 1);
}

static void test_nonprintable_char_shows_dot_and_freq_is_correct()
{
    HuffmanCompressor hc;
    std::vector<uint8_t> input = { 0, 0, 0, (uint8_t)'X' }; // NUL x3, 'X' x1

    std::string out = captureStdout(&callDebug, hc, input);

    auto rows = parsePrintedDictionary(out);
    assert(rows.count(0) == 1);
    assert(rows.count((int)'X') == 1);

    assert(rows[0].freq == 3);
    assert(rows[(int)'X'].freq == 1);

    // debugPrintDictionary prints '.' for non-printable
    assert(rows[0].ch == ".");
}

int main()
{
    test_empty_input();
    test_single_symbol_code_is_0();
    test_two_symbols_have_freqs_and_prefix_free_codes();
    test_nonprintable_char_shows_dot_and_freq_is_correct();

    std::cout << "[OK] HuffmanCompressor::debugPrintDictionary tests passed.\n";
    return 0;
}