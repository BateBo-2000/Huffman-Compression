#pragma once
#include <sstream>
#include <string>
#include <vector>

/*
Source:
https://www.geeksforgeeks.org/cpp/split-a-sentence-into-words-in-cpp/
*/


inline std::vector<std::string> splitWords(const std::string& text) {
    std::istringstream iss(text);
    std::vector<std::string> words;
    std::string w;

    while (iss >> w) {
        words.push_back(w);
    }
    return words;
}