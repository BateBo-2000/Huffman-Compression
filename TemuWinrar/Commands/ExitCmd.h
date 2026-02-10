#pragma once
#include "Command.h"
#include "../Execution/Invoker.h"

class ExitCmd : public Command {
public:
    explicit ExitCmd(Invoker& inv)
        : Command("exit"), invoker(inv) {
    }

    void execute(const std::vector<std::string>& args) override {
        invoker.requestExit();
    }

    std::string description() const override {
        return "Exit the application.";
    }

private:
    Invoker& invoker;
};