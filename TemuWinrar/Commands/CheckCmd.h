#pragma once
#include "Command.h"

class UserInterface;
class ArchiveMaster;

class CheckCmd : public Command {
public:
    CheckCmd(UserInterface& ui, ArchiveMaster& archive);

    void execute(const std::vector<std::string>& args) override;
    std::string description() const override;

private:
    UserInterface& ui;
    ArchiveMaster& archive;
};
