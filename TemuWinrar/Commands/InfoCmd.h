#pragma once
#include "Command.h"

class UserInterface;
class Archive;

class InfoCmd : public Command {
public:
    InfoCmd(UserInterface& ui, Archive& archive);

    void execute(const std::vector<std::string>& args) override;
    std::string description() const override;

private:
    UserInterface& ui;
    Archive& archive;
};
