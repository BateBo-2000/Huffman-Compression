#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm> //min

class HuffmanCompressor {
    static constexpr size_t ASCII = 256;
    static constexpr size_t MAX_NODES = 511; //2*256-1 = 511

public:
    struct HuffmanResult {
        uint64_t freq[HuffmanCompressor::ASCII];    //dictionary
        std::vector<uint8_t> compressed;            //data
    };

#ifdef DEBUG
    void debugPrintDictionary(const std::vector<uint8_t>& input);
#endif // DEBUG


    

    std::vector<uint8_t> compress(const std::vector<uint8_t>& input);
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& blob);

private:

    struct Dictionary
    {
        uint64_t originalSize;              //size decompressed
        uint64_t freq[ASCII];               //freq table
        std::vector<bool> codes[ASCII];     //array of 256 vectors that represent codes for every character (safest)


        Dictionary(): originalSize(0)
        {
            for (int i = 0; i < ASCII; i++) freq[i] = 0;
        }
    };


    struct Node {
        int symbol;       // -1 = sum of freq !-1 -> is a leaf
        uint64_t freq;      
        int minSymbol;    //look at NodeCmp

        Node* left;
        Node* right;

        Node(int s, uint64_t f)
            : symbol(s), freq(f), minSymbol(s), left(nullptr), right(nullptr) {
        }

        Node(Node* l, Node* r)
            : symbol(-1),
            freq(l->freq + r->freq),
            minSymbol(std::min(l->minSymbol, r->minSymbol)),
            left(l), right(r) {
        }
    };

    // comparator for the heap - see buildTreeFromFreq
    struct NodeLess {
        bool operator()(const Node* a, const Node* b) const {
            if (a->freq != b->freq) return a->freq < b->freq;
            return a->minSymbol < b->minSymbol;
        }
    };

private:
    void createDictionary(Dictionary& d, const std::vector<uint8_t>& input);

    //helpers
    void getFreq(uint64_t freq[ASCII], const std::vector<uint8_t>& input);
    Node* buildTreeFromFreq(const uint64_t freq[ASCII]);
    void deleteTree(Node* root);

    void buildCodes(Node* root, std::vector<bool>& path, Dictionary& d);
};