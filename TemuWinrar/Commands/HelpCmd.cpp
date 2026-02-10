#include "HelpCmd.h"
#include "../Execution/Invoker.h"
#include "../UI/UserInterface.h"
#include "Command.h"

#include <sstream>
#include <stdexcept>

HelpCommand::HelpCommand(const Invoker& inv, UserInterface& ui)
    : Command("help"), invoker(inv), ui(ui)
{
}

void HelpCommand::execute(const std::vector<std::string>& args)
{
    if (args.size() == 1) {
        printAll();
        return;
    }

    if (args.size() == 2) {
        printOne(args[1]);
        return;
    }

    throw std::invalid_argument("Usage: help OR help <command>");
}

std::string HelpCommand::description() const
{
    return "Shows available commands or help for a specific command.";
}

void HelpCommand::printAll() const
{
    std::ostringstream out;
    out << "Available commands:\n";

    const auto& cmds = invoker.getCommands();
    for (const auto* c : cmds) {
        if (!c) continue;
        out << "  - " << c->name() << "\n";
    }

    out << "\nTo get help for a command add it as an arg: help <cmd name>";
    ui.inform(out.str());
}

void HelpCommand::printOne(const std::string& cmdName) const
{
    const std::vector<Command*>& cmds = invoker.getCommands();

    for (int i = 0; i < cmds.size();i++) {
        if (cmds[i] && cmds[i]->name() == cmdName) {
            ui.inform(cmds[i]->name() + " - " + cmds[i]->description());
            return;
        }
    }

    throw std::invalid_argument("Unknown command: " + cmdName);
}
