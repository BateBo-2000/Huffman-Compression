#pragma once
#include "Command.h"

class UserInterface;
class Archive;

class UnzipCmd : public Command {
public:
    UnzipCmd(UserInterface& ui, Archive& archive);

    void execute(const std::vector<std::string>& args) override;
    std::string description() const override;

private:
    UserInterface& ui;
    Archive& archive;
};
