#pragma once
#include "Command.h"

class Invoker;
class UserInterface;

class HelpCmd : public Command {
public:
    HelpCmd(const Invoker& inv, UserInterface& ui);

    void execute(const std::vector<std::string>& args) override;
    std::string description() const override;

private:
    void printAll() const;
    void printOne(const std::string& cmdName) const;

private:
    const Invoker& invoker;
    UserInterface& ui;
};
