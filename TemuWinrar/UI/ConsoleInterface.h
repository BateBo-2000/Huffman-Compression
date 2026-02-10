#pragma once
#include "UserInterface.h"

#include <iostream>
#include <string>

class ConsoleInterface : public UserInterface {
public:
    void ask(const std::string& question) override {
        printColored(question, Color::Blue, true);
        printColored("> ", Color::White, false); //flex
        std::cout << std::flush;
    }

    std::string readLine() override {
        std::string line;
        printColored("> ", Color::White, false);
        std::getline(std::cin, line);
        return line;
    }

    void inform(const std::string& info) override {
        printColored(info, Color::White, true);
    }

    void warn(const std::string& warning) override {
        printColored(warning, Color::Yellow, true);
    }

    void error(const std::string& errorMsg) override {
        printColored(errorMsg, Color::Red, true);
    }

private:
    enum class Color {
        White = 37,
        Yellow = 33,
        Red = 31,
        Blue = 34,
        Reset = 0
    };

    static void printColored(const std::string& text, Color c, bool newline) {
        std::cout << "\033[" << (int)c << "m"
            << text
            << "\033[" << (int)Color::Reset << "m";
        if (newline) std::cout << '\n';
    }
};
