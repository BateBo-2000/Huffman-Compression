#include "HuffmanCompressor.h"
#include "../../Utils/StaticBinaryHeap.h"

#include <utility> //swap





#include <iostream>
#include <iomanip>
#include <cctype>

void HuffmanCompressor::debugPrintDictionary(const std::vector<uint8_t>& input)
{
    Dictionary d;
    createDictionary(d, input);

    std::cout << "===== HUFFMAN DICTIONARY =====\n";
    std::cout << "Original size: " << d.originalSize << "\n\n";

    std::cout << std::left
        << std::setw(8) << "Byte"
        << std::setw(8) << "Char"
        << std::setw(12) << "Freq"
        << "Code\n";
    std::cout << "----------------------------------------\n";

    for (size_t i = 0; i < ASCII; ++i)
    {
        if (d.freq[i] == 0) continue;

        char ch = (char)i;
        std::string chStr;
        if (std::isprint((unsigned char)ch))
            chStr = std::string(1, ch);
        else
            chStr = ".";

        std::cout << std::setw(8) << i
            << std::setw(8) << chStr
            << std::setw(12) << d.freq[i];

 
        for (size_t b = 0; b < d.codes[i].size(); ++b)
            std::cout << (d.codes[i][b] ? '1' : '0');

        std::cout << "\n";
    }

    std::cout << "========================================\n";
}





void HuffmanCompressor::createDictionary(HuffmanCompressor::Dictionary& d, const std::vector<uint8_t>& input) {
	d.originalSize = input.size();

	getFreq(d.freq, input);                                 //stores them into the dict

    for (size_t i = 0; i < ASCII; ++i) d.codes[i].clear();  //wipes the codes clean
    Node* root = buildTreeFromFreq(d.freq);

    std::vector<bool> path;
    buildCodes(root, path, d);

    deleteTree(root);
}


void HuffmanCompressor::getFreq(uint64_t freq[ASCII], const std::vector<uint8_t>& data)
{
	for (size_t i = 0; i < ASCII; i++)
		freq[i] = 0;

	for (size_t i = 0; i < data.size(); i++)
		++freq[data[i]];
}


HuffmanCompressor::Node* HuffmanCompressor::buildTreeFromFreq(const uint64_t freq[ASCII])
{
    StaticBinaryHeap<Node*, MAX_NODES, NodeLess> heap;

    for (int i = 0; i < (int)ASCII; ++i) {
        if (freq[i] > 0) {
            heap.insert(new Node(i, freq[i]));
        }
    }

    if (heap.empty()) return nullptr;

    NodeLess cmp;   //cleaner this way i think
    while (heap.getSize() > 1) {
        Node* a = heap.extract();
        Node* b = heap.extract();


        if (cmp(b, a))
            std::swap(a, b);

        heap.insert(new Node(a, b));
    }

    return heap.extract();
}

void HuffmanCompressor::deleteTree(Node* root)
{
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

void HuffmanCompressor::buildCodes(HuffmanCompressor::Node* root, std::vector<bool>& path, Dictionary& d)
{
    if (!root) return;

    // leaf
    if (!root->left && !root->right) {
        int s = root->symbol;
        d.codes[s].clear();

        if (path.empty()) d.codes[s].push_back(false);
        else d.codes[s] = path;

        return;
    }

    if (root->left) {
        path.push_back(false);
        buildCodes(root->left, path, d);
        path.pop_back();
    }
    if (root->right) {
        path.push_back(true);
        buildCodes(root->right, path, d);
        path.pop_back();
    }
}

