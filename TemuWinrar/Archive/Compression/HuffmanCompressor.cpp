#include "HuffmanCompressor.h"
#include "../../Utils/StaticBinaryHeap.h"
#include "../BitsIO/BitsWriter.h"
#include "../BitsIO/BitReader.h"
#include <utility> //swap





// just for debug function
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



//Helpre: to be reused - look compress
template <typename T>   
static void appendPOD(std::vector<uint8_t>& out, const T& value)
{
    size_t oldSize = out.size();
    out.resize(oldSize + sizeof(T));
    std::memcpy(out.data() + oldSize, &value, sizeof(T));
}

std::vector<uint8_t> HuffmanCompressor::compress(const std::vector<uint8_t>& input)
{
    Dictionary d;
    createDictionary(d, input);

    std::vector<uint8_t> blob;
    blob.reserve(sizeof(uint64_t) + ASCII * sizeof(uint64_t) + input.size());

        
    appendPOD<uint64_t>(blob, d.originalSize);  // write freq table

    
    for (size_t i = 0; i < ASCII; i++)          // write freq table
    {
        appendPOD<uint64_t>(blob, d.freq[i]);
    }

    
    BitWriter bw(blob);                         

    for (size_t i = 0; i < input.size(); i++)   // write the actual data
    {
        uint8_t byte = input[i];
        bw.writeBits(d.codes[byte]);
    }

    bw.flushZeroPad();

    return blob;
}

template <typename T>   // to be reused - look compress/decompress
static void readPOD(const std::vector<uint8_t>& in, size_t& offset, T& outValue)
{
    if (offset + sizeof(T) > in.size())
        throw std::runtime_error("Huffman decompress: blob too small while reading header");

    std::memcpy(&outValue, in.data() + offset, sizeof(T));
    offset += sizeof(T);
}

std::vector<uint8_t> HuffmanCompressor::decompress(const std::vector<uint8_t>& blob)
{
    // Header size: originalSize + freq[256]
    const size_t headerSize = sizeof(uint64_t) + ASCII * sizeof(uint64_t);
    if (blob.size() < headerSize)
        throw std::runtime_error("Huffman decompress: blob too small (missing header)");


    size_t offset = 0;

    
    uint64_t originalSize = 0;                          //reading originalSize             
    readPOD<uint64_t>(blob, offset, originalSize);

    uint64_t freq[ASCII];
    for (size_t i = 0; i < ASCII; i++)                  //reading the frequency table
        readPOD<uint64_t>(blob, offset, freq[i]);

    std::vector<uint8_t> output;                        
    output.reserve((size_t)originalSize);               //should be exactly that when decompressed     

    if (originalSize == 0)                              //if file is empty
        return output;

    Node* root = buildTreeFromFreq(freq);               

    if (root == nullptr){
        deleteTree(root);
        throw std::runtime_error("Huffman decompress: invalid dictionary (empty tree for non-empty data)");
    }

    //if root is leaf
    if (!root->left && !root->right)
    {
        if (root->symbol < 0 || root->symbol >= (int)ASCII) {
            deleteTree(root);
            throw std::runtime_error("Huffman decompres: invalid leaf symbol");
        }

        uint8_t symbol = (uint8_t)root->symbol;
        for (uint64_t i = 0; i < originalSize; i++)
            output.push_back(symbol);

        deleteTree(root);
        return output;
    }

    
    BitReader br(blob, offset);                     //bitStream starts at the offset bytes

    for (uint64_t produced = 0; produced < originalSize; produced++)
    {
        Node* cur = root;

        
        while (cur->left || cur->right)             // walk until leaf
        {
            bool bit = false;
            if (!br.readBit(bit)) {
                deleteTree(root);
                throw std::runtime_error("Huffman decompress: unexpected end of bitstream");
            }

            cur = bit ? cur->right : cur->left;

            if (cur == nullptr) {
                deleteTree(root);
                throw std::runtime_error("Hufman decompress: corrupted bitstream/tree (null child)");
            }
                
        }

        if (cur->symbol < 0 || cur->symbol >= (int)ASCII) {
            deleteTree(root);
            throw std::runtime_error("Huffman decompress: invalid leaf symbol");
        }

        output.push_back((uint8_t)cur->symbol);     //this is a leaf 
    }

    deleteTree(root);
    return output;
}


void HuffmanCompressor::createDictionary(HuffmanCompressor::Dictionary& d, const std::vector<uint8_t>& input) {
	d.originalSize = input.size();

	getFreq(d.freq, input);                                 //stores them into the dict

    for (size_t i = 0; i < ASCII; ++i) d.codes[i].clear();
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

